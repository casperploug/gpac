/*
 *			GPAC - Multimedia Framework C SDK
 *
 *			Authors: Jean le Feuvre
 *				Copyright (c) 2005-200X ENST
 *					All rights reserved
 *
 *  This file is part of GPAC / M2TS reader module
 *
 *  GPAC is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  GPAC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <gpac/modules/service.h>
#include <gpac/modules/codec.h>
#include <gpac/mpegts.h>
#include <gpac/thread.h>
#include <gpac/network.h>
#include <gpac/constants.h>

#ifndef GPAC_DISABLE_MPEG2TS

static const char * MIMES[] = { "video/mpeg-2", "video/mp2t", "video/mpeg", NULL};

typedef struct {
	char *fragment;
	u32 id;
	/*if only pid is requested*/
	u32 pid;
} M2TSIn_Prog;

typedef struct 
{
	GF_M2TS_Demuxer *ts;

	GF_InputService *owner;

	GF_ClientService *service;

	Bool ts_setup;
	Bool request_all_pids;

	Bool epg_requested;
	Bool has_eit;
	LPNETCHANNEL eit_channel;

	GF_Mutex *mx;

	Bool mpeg4on2_scene_only;
	char * network_buffer;
	u32 network_buffer_size;

}M2TSIn;


static void M2TS_GetNetworkType(GF_InputService *plug,M2TSIn *reader);

static Bool M2TS_CanHandleURL(GF_InputService *plug, const char *url)
{
	char *sExt;
        if (!plug || !url)
          return 0;
	if (!strnicmp(url, "udp://", 6)
		|| !strnicmp(url, "mpegts-udp://", 13)
		|| !strnicmp(url, "mpegts-tcp://", 13)
#ifdef GPAC_HAS_LINUX_DVB
		|| !strnicmp(url, "dvb://", 6)
#endif
	) {
  	return 1;
	}

	sExt = strrchr(url, '.');
	{
		int i=0;
		for (i = 0 ; NULL != MIMES[i]; i++)
			if (gf_term_check_extension(plug, MIMES[i], "ts m2t dmb", "MPEG-2 TS", sExt))
				return 1;
	}
	return 0;
}

static Bool M2TS_CanHandleURLInService(GF_InputService *plug, const char *url)
{
	Bool ret = 0;
	M2TSIn *m2ts;
        if (!plug || !url)
          return 0;
        m2ts = (M2TSIn *)plug->priv;
        if (!m2ts)
          return 0;

#ifdef GPAC_HAS_LINUX_DVB
	if (!stricmp(url, "dvb://EPG")) return 1;
	if (!strnicmp(url, "dvb://", 6)) {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[DVBIn] Checking reuse of the same tuner for %s\n", url));
		const char *chan_conf = gf_modules_get_option((GF_BaseInterface *)plug, "DVB", "ChannelsFile");
		if (!chan_conf) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[DVBIn] Cannot locate channel configuration file\n"));
			ret = 0;
		}

		/* if the tuner is already tuned to the same frequence, nothing needs to be done */
		else if (m2ts->ts->tuner->freq != 0) {
			char *frag = strchr(url, '#');
			if (frag) frag[0] = 0;
			if (m2ts->ts->tuner->freq == gf_dvb_get_freq_from_url(chan_conf, url)) {
				GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[DVBIn] Reusing the same tuner for %s\n", url));
				ret = 1;
			}
			if (frag) frag[0] = '#';
		}
	} else
#endif
	if (!strnicmp(url, "udp://", 6)
		|| !strnicmp(url, "mpegts-udp://", 13)
		|| !strnicmp(url, "mpegts-tcp://", 13))
	{
		/* TODO: check IP address ...*/
		ret = 0;
	} else {
		char *frag = strchr(url, '#');
		if (frag) frag[0] = 0;
		if (!strlen(url) || !strcmp(url, m2ts->ts->filename)) {
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[DVBIn] Reusing the same input file for %s\n", url));
 			ret = 1;
		}
		if (frag) frag[0] = '#';
	}
	return ret;
}

static GF_ObjectDescriptor *MP2TS_GetOD(M2TSIn *m2ts, GF_M2TS_PES *stream, char *dsi, u32 dsi_size, u32 *streamType)
{
	GF_ObjectDescriptor *od;
	GF_ESD *esd;

	/*create a stream description for this channel*/
	esd = gf_odf_desc_esd_new(0);
	esd->ESID = stream->mpeg4_es_id ? stream->mpeg4_es_id : stream->pid;

	switch (stream->stream_type) {
	case GF_M2TS_VIDEO_MPEG1:
		esd->decoderConfig->streamType = GF_STREAM_VISUAL;
		esd->decoderConfig->objectTypeIndication = GPAC_OTI_VIDEO_MPEG1;
		break;
	case GF_M2TS_VIDEO_MPEG2:
		esd->decoderConfig->streamType = GF_STREAM_VISUAL;
		esd->decoderConfig->objectTypeIndication = GPAC_OTI_VIDEO_MPEG2_422;
		break;
	case GF_M2TS_VIDEO_MPEG4:
		esd->decoderConfig->streamType = GF_STREAM_VISUAL;
		esd->decoderConfig->objectTypeIndication = GPAC_OTI_VIDEO_MPEG4_PART2;
		break;
	case GF_M2TS_VIDEO_H264:
		esd->decoderConfig->streamType = GF_STREAM_VISUAL;
		esd->decoderConfig->objectTypeIndication = GPAC_OTI_VIDEO_AVC;
		break;
	case GF_M2TS_VIDEO_VC1:
		esd->decoderConfig->streamType = GF_STREAM_VISUAL;
		esd->decoderConfig->objectTypeIndication = GPAC_OTI_VIDEO_SMPTE_VC1;
		break;
	case GF_M2TS_AUDIO_MPEG1:
		esd->decoderConfig->streamType = GF_STREAM_AUDIO;
		esd->decoderConfig->objectTypeIndication = GPAC_OTI_AUDIO_MPEG1;
		break;
	case GF_M2TS_AUDIO_MPEG2:
		esd->decoderConfig->streamType = GF_STREAM_AUDIO;
		esd->decoderConfig->objectTypeIndication = GPAC_OTI_AUDIO_MPEG2_PART3;
		break;
	case GF_M2TS_AUDIO_LATM_AAC:
	case GF_M2TS_AUDIO_AAC:
		if (!dsi) {
			/*discard regulate until we fetch the AAC config*/
			m2ts->ts->file_regulate = 0;
			/*turn on parsing*/
			gf_m2ts_set_pes_framing(stream, GF_M2TS_PES_FRAMING_DEFAULT);
			gf_odf_desc_del((GF_Descriptor *)esd);
			return NULL;
		}
		esd->decoderConfig->streamType = GF_STREAM_AUDIO;
		esd->decoderConfig->objectTypeIndication = GPAC_OTI_AUDIO_AAC_MPEG4;
		break;
	case GF_M2TS_SYSTEMS_MPEG4_SECTIONS:
	default:
		gf_odf_desc_del((GF_Descriptor *)esd);
		return NULL;
	}
	esd->decoderConfig->bufferSizeDB = 0;

	/*we only use AUstart indicator*/
	esd->slConfig->useAccessUnitStartFlag = 1;
	esd->slConfig->useAccessUnitEndFlag = 0;
	esd->slConfig->useRandomAccessPointFlag = 1;
	esd->slConfig->AUSeqNumLength = 0;
	esd->slConfig->timestampResolution = 90000;

	/*ASSIGN PCR here*/
	esd->OCRESID = stream->program->pcr_pid;
	if (stream->pid == stream->program->pcr_pid) {
		esd->slConfig->OCRResolution = 27000000;
	}

	/*decoder config*/
	if (dsi) {
		esd->decoderConfig->decoderSpecificInfo->data = gf_malloc(sizeof(char)*dsi_size);
		memcpy(esd->decoderConfig->decoderSpecificInfo->data, dsi, sizeof(char)*dsi_size);
		esd->decoderConfig->decoderSpecificInfo->dataLength = dsi_size;
	}

	/*declare object to terminal*/
	od = (GF_ObjectDescriptor*)gf_odf_desc_new(GF_ODF_OD_TAG);
	gf_list_add(od->ESDescriptors, esd);
	od->objectDescriptorID = 0;
	if (streamType) *streamType = esd->decoderConfig->streamType;
	/*remember program number for service/program selection*/
	od->ServiceID = stream->program->number;
	od->service_ifce = m2ts->owner;
	return od;
}

static void MP2TS_DeclareStream(M2TSIn *m2ts, GF_M2TS_PES *stream, char *dsi, u32 dsi_size)
{
	GF_ObjectDescriptor *od = MP2TS_GetOD(m2ts, stream, dsi, dsi_size, NULL);
	if (!od) return;
	/*declare but don't regenerate scene*/
	gf_term_add_media(m2ts->service, (GF_Descriptor*)od, 1);
}

static void MP2TS_SetupProgram(M2TSIn *m2ts, GF_M2TS_Program *prog, Bool regenerate_scene, Bool no_declare)
{
	u32 i, count;
	Bool force_declare_ods = 0;

	count = gf_list_count(prog->streams);
#ifdef GPAC_HAS_LINUX_DVB
	if (m2ts->ts->tuner) {
		Bool found = 0;
		for (i=0; i<count; i++) {
			GF_M2TS_PES *pes = gf_list_get(prog->streams, i);
			if (pes->pid==m2ts->ts->tuner->vpid) found = 1;
			else if (pes->pid==m2ts->ts->tuner->apid) found = 1;
		}
		if (!found) return;
	}
#endif
	if (m2ts->ts->file || m2ts->ts->dnload) m2ts->ts->file_regulate = no_declare ? 0 : 1;

	if (prog->pmt_iod && (prog->pmt_iod->tag==GF_ODF_IOD_TAG) && (((GF_InitialObjectDescriptor*)prog->pmt_iod)->OD_profileAndLevel==GPAC_MAGIC_OD_PROFILE_FOR_MPEG4_SIGNALING)) {
		force_declare_ods = 1;
	}
	for (i=0; i<count; i++) {
		GF_M2TS_ES *es = gf_list_get(prog->streams, i);
		if (es->pid==prog->pmt_pid) continue;
		/*move to skip mode for all ES until asked for playback*/
		if (!es->user)
			gf_m2ts_set_pes_framing((GF_M2TS_PES *)es, GF_M2TS_PES_FRAMING_SKIP);

		if (!prog->pmt_iod && !no_declare) {
			MP2TS_DeclareStream(m2ts, (GF_M2TS_PES *)es, NULL, 0);
		} else if (force_declare_ods) {
			if ((es->stream_type!=GF_M2TS_SYSTEMS_MPEG4_PES) && (es->stream_type!=GF_M2TS_SYSTEMS_MPEG4_SECTIONS)) {
				MP2TS_DeclareStream(m2ts, (GF_M2TS_PES *)es, NULL, 0);
			}
		}
	}

	/*force scene regeneration*/
	if (!prog->pmt_iod && regenerate_scene)
		gf_term_add_media(m2ts->service, NULL, 0);
}

static void MP2TS_SendPacket(M2TSIn *m2ts, GF_M2TS_PES_PCK *pck)
{
	GF_SLHeader slh;

	/*pcr not initialized, don't send any data*/
	if (! pck->stream->program->first_dts) return;
	if (!pck->stream->user) return;

	memset(&slh, 0, sizeof(GF_SLHeader));
	slh.accessUnitStartFlag = (pck->flags & GF_M2TS_PES_PCK_AU_START) ? 1 : 0;
	if (slh.accessUnitStartFlag) {
#if 0
		slh.OCRflag = 1;
		slh.m2ts_pcr = 1;
		slh.objectClockReference = pck->stream->program->last_pcr_value;
#else
		slh.OCRflag = 0;
#endif
		slh.compositionTimeStampFlag = 1;
		slh.compositionTimeStamp = pck->PTS;
		if (pck->DTS) {
			slh.decodingTimeStampFlag = 1;
			slh.decodingTimeStamp = pck->DTS;
		}
		slh.randomAccessPointFlag = (pck->flags & GF_M2TS_PES_PCK_RAP) ? 1 : 0;
	}
	gf_term_on_sl_packet(m2ts->service, pck->stream->user, pck->data, pck->data_len, &slh, GF_OK);
}

static GFINLINE void MP2TS_SendSLPacket(M2TSIn *m2ts, GF_M2TS_SL_PCK *pck)
{
	GF_SLHeader SLHeader, *slh = NULL;
	u32 SLHdrLen = 0;

	/*build a SL Header*/
	if (((GF_M2TS_ES*)pck->stream)->slcfg) {
		gf_sl_depacketize(((GF_M2TS_ES*)pck->stream)->slcfg, &SLHeader, pck->data, pck->data_len, &SLHdrLen);
		SLHeader.m2ts_version_number_plus_one = pck->version_number + 1;
		slh = &SLHeader;
	}
	gf_term_on_sl_packet(m2ts->service, pck->stream->user, pck->data+SLHdrLen, pck->data_len-SLHdrLen, slh, GF_OK);
}

static GF_ObjectDescriptor *M2TS_GenerateEPG_OD(M2TSIn *m2ts)
{
	/* declaring a special stream for displaying eit */
	GF_ObjectDescriptor *od;
	GF_ESD *esd;

	/*create a stream description for this channel*/
	esd = gf_odf_desc_esd_new(0);
	esd->ESID = GF_M2TS_PID_EIT_ST_CIT;
	esd->OCRESID = GF_M2TS_PID_EIT_ST_CIT;
	esd->decoderConfig->streamType = GF_STREAM_PRIVATE_SCENE;
	esd->decoderConfig->objectTypeIndication = GPAC_OTI_PRIVATE_SCENE_EPG;
	esd->decoderConfig->bufferSizeDB = 0;

	/*we only use AUstart indicator
	esd->slConfig->useAccessUnitStartFlag = 1;
	esd->slConfig->useAccessUnitEndFlag = 0;
	esd->slConfig->useRandomAccessPointFlag = 1;
	esd->slConfig->AUSeqNumLength = 0;
	esd->slConfig->timestampResolution = 90000;*/

	/*declare object to terminal*/
	od = (GF_ObjectDescriptor*)gf_odf_desc_new(GF_ODF_OD_TAG);
	gf_list_add(od->ESDescriptors, esd);
	od->objectDescriptorID = 0;
	od->service_ifce = m2ts->owner;
	return od;
}

static void M2TS_FlushRequested(M2TSIn *m2ts)
{
	u32 i, j, req_prog_count, count, prog_id, found;

	gf_mx_p(m2ts->mx);

	found = 0;
	count = gf_list_count(m2ts->ts->requested_pids);
	for (i=0; i<count; i++) {
		M2TSIn_Prog *req_pid = gf_list_get(m2ts->ts->requested_pids, i);
		GF_M2TS_ES *es = m2ts->ts->ess[req_pid->pid];
		if (es==NULL) continue;

		/*move to skip mode for all PES until asked for playback*/
		if (!(es->flags & GF_M2TS_ES_IS_SECTION) && !es->user)
			gf_m2ts_set_pes_framing((GF_M2TS_PES *)es, GF_M2TS_PES_FRAMING_SKIP);
		MP2TS_DeclareStream(m2ts, (GF_M2TS_PES *)es, NULL, 0);
		gf_list_rem(m2ts->ts->requested_pids, i);
		gf_free(req_pid);
		i--;
		count--;
		found++;
	}
	req_prog_count = gf_list_count(m2ts->ts->requested_progs);
	for (i = 0; i < req_prog_count; i++) {
		M2TSIn_Prog *req_prog = gf_list_get(m2ts->ts->requested_progs, i);
		prog_id = atoi(req_prog->fragment);
		count = gf_list_count(m2ts->ts->SDTs);
		for (j=0; j<count; j++) {
			GF_M2TS_SDT *sdt = gf_list_get(m2ts->ts->SDTs, j);
			if (!stricmp(sdt->service, req_prog->fragment)) req_prog->id = sdt->service_id;
			else if (sdt->service_id==prog_id)  req_prog->id = sdt->service_id;
		}
		if (req_prog->id) {
			GF_M2TS_Program *ts_prog;
			count = gf_list_count(m2ts->ts->programs);
			for (j=0; j<count; j++) {
				ts_prog = gf_list_get(m2ts->ts->programs, j);
				if (ts_prog->number==req_prog->id) {
					MP2TS_SetupProgram(m2ts, ts_prog, 0, 0);
					found++;
					gf_free(req_prog->fragment);
					gf_free(req_prog);
					gf_list_rem(m2ts->ts->requested_progs, i);
					req_prog_count--;
					i--;
					break;
				}
			}
		}
	}

	if (m2ts->epg_requested) {
		if (!m2ts->has_eit) {
			GF_ObjectDescriptor *od = M2TS_GenerateEPG_OD(m2ts);
			/*declare but don't regenerate scene*/
			gf_term_add_media(m2ts->service, (GF_Descriptor*)od, 0);
			m2ts->has_eit = 1;
		}
	} else {
		/*force scene regeneration only when EPG is not requested*/
		if (found)
			gf_term_add_media(m2ts->service, NULL, 0);
	}

	gf_mx_v(m2ts->mx);
}

static void M2TS_OnEvent(GF_M2TS_Demuxer *ts, u32 evt_type, void *param)
{
	M2TSIn *m2ts = (M2TSIn *) ts->user;
	switch (evt_type) {
	case GF_M2TS_EVT_PAT_UPDATE:
/*	example code showing how to forward an event from MPEG-2 TS input service to GPAC user*/
#if 0
		{
		GF_Event evt;
		evt.type = GF_EVENT_FORWARDED;
		evt.forwarded_event.forward_type = GF_EVT_FORWARDED_MPEG2;
		evt.forwarded_event.forward_type = GF_EVT_FORWARDED_MPEG2;
		evt.forwarded_event.service_event_type = evt_type;
		evt.forwarded_event.param = param;
		gf_term_on_service_event(m2ts->service, &evt);
	}
#endif
		break;
	case GF_M2TS_EVT_AIT_FOUND:
		{
		  GF_Event evt;
		  evt.type = GF_EVENT_FORWARDED;
		  evt.forwarded_event.forward_type = GF_EVT_FORWARDED_MPEG2;
		  evt.forwarded_event.service_event_type = evt_type;
		  evt.forwarded_event.param = param;		  
		  gf_term_on_service_event(m2ts->service, &evt);
		}
		break;
	case GF_M2TS_EVT_PAT_FOUND:
		/* In case the TS has one program, wait for the PMT to send connect, in case of IOD in PMT */		
		if (gf_list_count(m2ts->ts->programs) != 1)
			gf_term_on_connect(m2ts->service, NULL, GF_OK);
		{
			/* Send the TS to the a user if needed. Useful to check the number of received programs*/
			GF_Event evt;
			evt.type = GF_EVENT_FORWARDED;
			evt.forwarded_event.forward_type = GF_M2TS_EVT_PAT_FOUND;
			evt.forwarded_event.service_event_type = evt_type;
			evt.forwarded_event.param = ts;
			gf_term_on_service_event(m2ts->service, &evt);		
			
		}
		break;
	case GF_M2TS_EVT_PMT_FOUND:
		if (gf_list_count(m2ts->ts->programs) == 1)
			gf_term_on_connect(m2ts->service, NULL, GF_OK);

		/*do not declare if  single program was requested for playback*/
		MP2TS_SetupProgram(m2ts, param, m2ts->request_all_pids, m2ts->request_all_pids ? 0 : 1);
		M2TS_FlushRequested(m2ts);
		break;
	case GF_M2TS_EVT_PMT_REPEAT:
//	case GF_M2TS_EVT_PMT_UPDATE:
		M2TS_FlushRequested(m2ts);
		break;
	case GF_M2TS_EVT_SDT_REPEAT:
	case GF_M2TS_EVT_SDT_UPDATE:
	case GF_M2TS_EVT_SDT_FOUND:
		M2TS_FlushRequested(m2ts);
		break;
	case GF_M2TS_EVT_DVB_GENERAL:
		if (m2ts->eit_channel) {
			GF_M2TS_SL_PCK *pck = (GF_M2TS_SL_PCK *)param;
			gf_term_on_sl_packet(m2ts->service, m2ts->eit_channel, pck->data, pck->data_len, NULL, GF_OK);
		}
		break;
	case GF_M2TS_EVT_PES_PCK:
		MP2TS_SendPacket(m2ts, param);
		break;
	case GF_M2TS_EVT_SL_PCK:
		MP2TS_SendSLPacket(m2ts, param);
		break;
	case GF_M2TS_EVT_AAC_CFG:
	{
		GF_M2TS_PES_PCK *pck = (GF_M2TS_PES_PCK*)param;
		if (!pck->stream->first_dts) {
			gf_m2ts_set_pes_framing(pck->stream, GF_M2TS_PES_FRAMING_SKIP_NO_RESET);
			MP2TS_DeclareStream(m2ts, pck->stream, pck->data, pck->data_len);
			if (ts->file || ts->dnload) ts->file_regulate = 1;
			pck->stream->first_dts=1;
			/*force scene regeneration*/
			gf_term_add_media(m2ts->service, NULL, 0);
		}
	}
		break;
	case GF_M2TS_EVT_PES_PCR:
		/*send pcr*/
		if (((GF_M2TS_PES_PCK *) param)->stream && ((GF_M2TS_PES_PCK *) param)->stream->user) {
			GF_SLHeader slh;
			memset(&slh, 0, sizeof(GF_SLHeader) );
			slh.OCRflag = 1;
			slh.m2ts_pcr = ( ((GF_M2TS_PES_PCK *) param)->flags & GF_M2TS_PES_PCK_DISCONTINUITY) ? 2 : 1;
			slh.objectClockReference = ((GF_M2TS_PES_PCK *) param)->PTS;
			gf_term_on_sl_packet(m2ts->service, ((GF_M2TS_PES_PCK *) param)->stream->user, NULL, 0, &slh, GF_OK);
		}
		((GF_M2TS_PES_PCK *) param)->stream->program->first_dts = 1;

		if ( ((GF_M2TS_PES_PCK *) param)->flags & GF_M2TS_PES_PCK_DISCONTINUITY) {
#if 0
			if (ts->pcr_last) {
				ts->pcr_last = ((GF_M2TS_PES_PCK *) param)->PTS;
				ts->stb_at_last_pcr = gf_sys_clock();
			}
#endif
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[M2TS In] PCR discontinuity - switching from old STB "LLD" to new one "LLD"\n", ts->pcr_last, ((GF_M2TS_PES_PCK *) param)->PTS));
			/*FIXME - we need to find a way to treat PCR discontinuities correctly while ignoring broken PCR discontinuities 
			seen in many HLS solutions*/
			return;
		}

		if (ts->file_regulate) {
			u64 pcr = ((GF_M2TS_PES_PCK *) param)->PTS;
			u32 stb = gf_sys_clock();
			if (ts->pcr_last) {
				s32 diff;
				if (pcr < ts->pcr_last) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[M2TS In] PCR "LLU" less than previous PCR "LLU"\n", ((GF_M2TS_PES_PCK *) param)->PTS, ts->pcr_last));
					ts->pcr_last = pcr;
					ts->stb_at_last_pcr = gf_sys_clock();
					diff = 0;
				} else {
					u64 pcr_diff = (pcr - ts->pcr_last);
					pcr_diff /= 27000;
					if (pcr_diff>500) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[M2TS In] PCR diff too big: "LLU" ms - PCR "LLU" - previous PCR "LLU" - error in TS ?\n", pcr_diff, ((GF_M2TS_PES_PCK *) param)->PTS, ts->pcr_last));
						diff = 100;
					} else {
						diff = (u32) pcr_diff - (stb - ts->stb_at_last_pcr);
					}
				}
				if (diff<0) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[M2TS In] Demux not going fast enough according to PCR (drift %d, pcr: "LLU", last pcr: "LLU")\n", diff, pcr, ts->pcr_last));
				} else if (diff>0) {
					u32 sleep_for=1;
#ifndef GPAC_DISABLE_LOG
					u32 nb_sleep=0;
#endif
					/*query buffer level, don't sleep if too low*/
					GF_NetworkCommand com;
					com.command_type = GF_NET_BUFFER_QUERY;
					while (ts->run_state) {
						gf_term_on_command(m2ts->service, &com, GF_OK);
						if (com.buffer.occupancy < M2TS_BUFFER_MAX) {
							GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[M2TS In] Demux not going to sleep: buffer occupancy %d ms\n", com.buffer.occupancy));
							break;
						}
						/*We don't sleep for the entire buffer occupancy, because we would take
						the risk of starving the audio chains. We try to keep buffers half full*/
#ifndef GPAC_DISABLE_LOG
						if (!nb_sleep) {
							GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[M2TS In] Demux going to sleep (buffer occupancy %d ms)\n", com.buffer.occupancy));
						}
						nb_sleep++;
#endif
						gf_sleep(sleep_for);
					}
#ifndef GPAC_DISABLE_LOG
					if (nb_sleep) {
						GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[M2TS In] Demux resume after %d ms - current buffer occupancy %d ms\n", sleep_for*nb_sleep, com.buffer.occupancy));
					}
#endif
					ts->nb_pck = 0;
					ts->pcr_last = pcr;
					ts->stb_at_last_pcr = gf_sys_clock();
				} else {
					GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[M2TS In] Demux drift according to PCR (drift %d, pcr: "LLD", last pcr: "LLD")\n", diff, pcr, ts->pcr_last));
				}
			} else {
				ts->pcr_last = pcr;
				ts->stb_at_last_pcr = gf_sys_clock();
			}
		}
		break;
	}
}

static void M2TS_OnEventPCR(GF_M2TS_Demuxer *ts, u32 evt_type, void *param)
{
	if (evt_type==GF_M2TS_EVT_PES_PCR) {
		M2TSIn *m2ts = ts->user;
		GF_M2TS_PES_PCK *pck = param;
		if (!ts->nb_playing) {
			ts->nb_playing = pck->stream->pid;
			ts->end_range = (u32) (pck->PTS / 90);
		} else if (ts->nb_playing == pck->stream->pid) {
			ts->start_range = (u32) (pck->PTS / 90);
		}
	}
}

void m2ts_net_io(void *cbk, GF_NETIO_Parameter *param)
{
	GF_Err e;
	M2TSIn *m2ts = (M2TSIn *) cbk;
        assert( m2ts );
	/*handle service message*/
	gf_term_download_update_stats(m2ts->ts->dnload);

	if (param->msg_type==GF_NETIO_DATA_TRANSFERED) {
		e = GF_EOS;
	} else if (param->msg_type==GF_NETIO_DATA_EXCHANGE) {
		e = GF_OK;
                assert( m2ts->ts);
                if (param->size > 0){
                  /*process chunk*/
                  assert(param->data);
		  if (m2ts->network_buffer_size < param->size){
			  m2ts->network_buffer = gf_realloc(m2ts->network_buffer, sizeof(char) * param->size);
			  m2ts->network_buffer_size = param->size;
		  }
		  assert( m2ts->network_buffer );
		  memcpy(m2ts->network_buffer, param->data, param->size);
                  gf_m2ts_process_data(m2ts->ts, m2ts->network_buffer, param->size);
                }

		/*if asked to regulate, wait until we get a play request*/
		if (m2ts->ts->run_state && !m2ts->ts->nb_playing && m2ts->ts->file_regulate) {
			while (m2ts->ts->run_state && !m2ts->ts->nb_playing && m2ts->ts->file_regulate) {
				gf_sleep(50);
				continue;
			}
		} else {
			gf_sleep(1);
		}
		if (!m2ts->ts->run_state) {
			if (m2ts->ts->dnload) 
				gf_term_download_del( m2ts->ts->dnload );
			m2ts->ts->dnload = NULL;
		}

	} else {
		e = param->error;
	}

	switch (e){
	  case GF_EOS:
	    gf_term_on_connect(m2ts->service, NULL, GF_OK);
	    return;
	  case GF_OK:
	    return;
	  default:
		if (!m2ts->ts_setup) {
			m2ts->ts_setup = 1;
		}
		GF_LOG( GF_LOG_ERROR, GF_LOG_CONTAINER,("[MPEGTSIn] : Error while getting data : %s\n", gf_error_to_string(e)));
		gf_term_on_connect(m2ts->service, NULL, e);
	}
}

static const char *M2TS_QueryNextFile(void *udta)
{
	GF_NetworkCommand param;
	GF_Err query_ret;
	M2TSIn *m2ts = (M2TSIn *) udta;
	assert(m2ts->owner);
	assert( m2ts->owner->query_proxy);

	param.command_type = GF_NET_SERVICE_QUERY_NEXT;
	param.url_query.next_url = NULL;
	query_ret = m2ts->owner->query_proxy(m2ts->owner, &param);
	if ((query_ret==GF_OK) && param.url_query.next_url){
		GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[M2TS In] Switching to next segment %s\n", param.url_query.next_url));
		return param.url_query.next_url;
	} else if (query_ret==GF_OK){
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[M2TS In] Cannot query next file: no file provided but no error raised\n"));
	} else {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[M2TS In] Cannot query next file: error: %s\n", gf_error_to_string(query_ret)));
	}	
	return NULL;
}


static GF_Err M2TS_ConnectService(GF_InputService *plug, GF_ClientService *serv, const char *url)
{
	GF_Err e;	
	M2TSIn *m2ts = plug->priv;

	M2TS_GetNetworkType(plug,m2ts);

	m2ts->owner = plug;
	m2ts->service = serv;
	if (m2ts->owner->query_proxy) {
		m2ts->ts->query_next = M2TS_QueryNextFile;
		m2ts->ts->udta_query = m2ts;
	}

	if (!strnicmp(url, "http://", 7)) {
		m2ts->ts->dnload = gf_term_download_new(m2ts->service, url, GF_NETIO_SESSION_NOT_THREADED | GF_NETIO_SESSION_NOT_CACHED, m2ts_net_io, m2ts);
		if (!m2ts->ts->dnload){
			gf_term_on_connect(m2ts->service, NULL, GF_NOT_SUPPORTED);
			return GF_OK;
		} else {
			e = TSDemux_DemuxPlay(m2ts->ts);
		}
	} else {
		e = TSDemux_Demux_Setup(m2ts->ts,url,0);
	}

	if (e) {
		gf_term_on_connect(m2ts->service, NULL, e);	
	}
	return GF_OK;
}

static GF_Err M2TS_CloseService(GF_InputService *plug)
{
	M2TSIn *m2ts = plug->priv;
	GF_M2TS_Demuxer* ts = m2ts->ts;

	TSDemux_CloseDemux(ts);	
	
	
	if (ts->dnload) gf_term_download_del(ts->dnload);
	ts->dnload = NULL;

	gf_term_on_disconnect(m2ts->service, NULL, GF_OK);
	return GF_OK;
}

static GF_Descriptor *M2TS_GetServiceDesc(GF_InputService *plug, u32 expect_type, const char *sub_url)
{
	M2TSIn *m2ts = plug->priv;
	GF_Descriptor *desc = NULL;
	char *frag;

	frag = sub_url ? strrchr(sub_url, '#') : NULL;
	if (frag) frag++;

	/* consider the channel name in DVB URL as a fragment */
	if (!frag && !strncmp(sub_url, "dvb://", 6)) {
		frag = (char*)sub_url + 6;
	}

	if (!frag) {
		m2ts->request_all_pids = 1;
	} else {
		M2TSIn_Prog *prog;

		/*we need exclusive access*/
		gf_mx_p(m2ts->mx);
		if (!strnicmp(frag, "pid=", 4)) {
			GF_SAFEALLOC(prog, M2TSIn_Prog);
			prog->pid = atoi(frag+4);
			gf_list_add(m2ts->ts->requested_pids, prog);
		} else if (!strnicmp(frag, "EPG", 3)) {
			m2ts->epg_requested = 1;
		} else {
			u32 i, count;
			count = gf_list_count(m2ts->ts->requested_progs);
			prog = NULL;
			for (i=0; i<count; i++) {
				prog = gf_list_get(m2ts->ts->requested_progs, i);
				if (!strcmp(prog->fragment, frag))
					break;
				prog = NULL;
			}
			if (!prog) {
				GF_SAFEALLOC(prog, M2TSIn_Prog);
				gf_list_add(m2ts->ts->requested_progs, prog);
				prog->fragment = gf_strdup(frag);
			}
		}
		gf_mx_v(m2ts->mx);
	}

	if (expect_type==GF_MEDIA_OBJECT_SCENE) {
		if (gf_list_count(m2ts->ts->programs) == 1) {
			GF_M2TS_Program *prog = gf_list_get(m2ts->ts->programs, 0);
			if (prog->pmt_iod) {
				gf_odf_desc_copy((GF_Descriptor *)prog->pmt_iod, &desc);
				return desc;
			}
		}
		if (m2ts->epg_requested) {
			GF_ObjectDescriptor *od = M2TS_GenerateEPG_OD(m2ts);
			m2ts->epg_requested = 0;
			return (GF_Descriptor *)od;
		} else {
			/*returning an empty IOD means "no scene description", let the terminal handle all media objects*/
			desc = gf_odf_desc_new(GF_ODF_IOD_TAG);
			((GF_ObjectDescriptor *) desc)->objectDescriptorID = 1;
			return desc;
		}
	}

	/* restart the thread if the same service is reused and if the previous thread terminated */
	if (m2ts->ts->run_state == 2) {
		m2ts->ts->file_regulate = 0;
		TSDemux_DemuxPlay(m2ts->ts);
	}

	return NULL;
}

static GF_Err M2TS_ConnectChannel(GF_InputService *plug, LPNETCHANNEL channel, const char *url, Bool upstream)
{
	u32 ES_ID;
	GF_Err e;
	M2TSIn *m2ts = plug->priv;

	e = GF_STREAM_NOT_FOUND;
	if (strstr(url, "ES_ID")) {
		sscanf(url, "ES_ID=%d", &ES_ID);

		/* In case there is a real IOD, we need to translate PID into ESID */
		if (gf_list_count(m2ts->ts->programs) == 1) {
			GF_M2TS_Program *prog = gf_list_get(m2ts->ts->programs, 0);
			if (prog->pmt_iod) {
				/* IOD is present */
				u32 i;
				for (i=0; i<GF_M2TS_MAX_STREAMS; i++) {
					GF_M2TS_PES *pes = (GF_M2TS_PES *)m2ts->ts->ess[i];
					if (!pes || (pes->pid==pes->program->pmt_pid)) continue;
					if (pes->mpeg4_es_id == ES_ID) {
						if (pes->user) {
							e = GF_SERVICE_ERROR;
							gf_term_on_connect(m2ts->service, channel, e);
							return e;
						} else {
							pes->user = channel;
							e = GF_OK;
							gf_term_on_connect(m2ts->service, channel, e);
							return e;
						}
					}
				}
				/* Stream not found */
				return e;
			}
		}

		/* No IOD */
		if (ES_ID == 18) {
			e = GF_OK; /* 18 is the PID of EIT packets */
			m2ts->eit_channel = channel;
		} else if ((ES_ID<GF_M2TS_MAX_STREAMS) && m2ts->ts->ess[ES_ID]) {
			GF_M2TS_PES *pes = (GF_M2TS_PES *)m2ts->ts->ess[ES_ID];
			if (pes->user) {
				e = GF_SERVICE_ERROR;
			} else {
				pes->user = channel;
				e = GF_OK;
			}
		}
	}
	gf_term_on_connect(m2ts->service, channel, e);
	return e;
}

static GF_M2TS_PES *M2TS_GetChannel(M2TSIn *m2ts, LPNETCHANNEL channel)
{
	u32 i;
	for (i=0; i<GF_M2TS_MAX_STREAMS; i++) {
		GF_M2TS_PES *pes = (GF_M2TS_PES *)m2ts->ts->ess[i];
		if (!pes || (pes->pid==pes->program->pmt_pid)) continue;
		if (pes->user == channel) return pes;
	}
	return NULL;
}
static GF_Err M2TS_DisconnectChannel(GF_InputService *plug, LPNETCHANNEL channel)
{
	M2TSIn *m2ts = plug->priv;
	GF_Err e = GF_STREAM_NOT_FOUND;
	GF_M2TS_PES *pes = M2TS_GetChannel(m2ts, channel);
	if (pes) {
		pes->user = NULL;
		e = GF_OK;
	}
	gf_term_on_disconnect(m2ts->service, channel, e);
	return GF_OK;
}

static GF_Err M2TS_ServiceCommand(GF_InputService *plug, GF_NetworkCommand *com)
{
	GF_M2TS_PES *pes;
	M2TSIn *m2ts = plug->priv;
	GF_M2TS_Demuxer *ts = m2ts->ts;	

	if (com->command_type==GF_NET_SERVICE_HAS_AUDIO) {
		char *frag = strchr(com->audio.base_url, '#');
		if (frag && !strnicmp(frag, "#pid=", 5)) return GF_NOT_SUPPORTED;
		return GF_OK;
	}
	if (!com->base.on_channel) return GF_NOT_SUPPORTED;
	switch (com->command_type) {
	/*we cannot pull complete AUs from the stream*/
	case GF_NET_CHAN_SET_PULL:
		return GF_NOT_SUPPORTED;
	/*we cannot seek stream by stream*/
	case GF_NET_CHAN_INTERACTIVE:
		return GF_NOT_SUPPORTED;
	case GF_NET_CHAN_BUFFER:
		com->buffer.max = M2TS_BUFFER_MAX;
		com->buffer.min = 0;
		return GF_OK;
	case GF_NET_CHAN_DURATION:
		com->duration.duration = ts->duration;
		return GF_OK;
	case GF_NET_CHAN_PLAY:
		pes = M2TS_GetChannel(m2ts, com->base.on_channel);
		if (!pes) {
			if (com->base.on_channel == m2ts->eit_channel) {
				return GF_OK;
			}
			return GF_STREAM_NOT_FOUND;
		}
		/*mark pcr as not initialized*/
		if (pes->program->pcr_pid==pes->pid) pes->program->first_dts=0;
		gf_m2ts_set_pes_framing(pes, GF_M2TS_PES_FRAMING_DEFAULT);
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[M2TSIn] Setting default reframing for PID %d\n", pes->pid));
		/*this is a multplex, only trigger the play command for the first stream activated*/
		if (!ts->nb_playing) {
			ts->start_range = (u32) (com->play.start_range*1000);
			ts->end_range = (com->play.end_range>0) ? (u32) (com->play.end_range*1000) : 0;
			/*start demuxer*/
			if (ts->run_state!=1) {
				return TSDemux_DemuxPlay(ts);
			}
		}
		ts->nb_playing++;
		return GF_OK;
	case GF_NET_CHAN_STOP:
		pes = M2TS_GetChannel(m2ts, com->base.on_channel);
		if (!pes) {
			if (com->base.on_channel == m2ts->eit_channel) {
				return GF_OK;
			}
			return GF_STREAM_NOT_FOUND;
		}

		gf_m2ts_set_pes_framing(pes, GF_M2TS_PES_FRAMING_SKIP);
		/* In case of EOS, we may receive a stop command after no one is playing */
		if (ts->nb_playing)
		  ts->nb_playing--;
		/*stop demuxer*/
		if (!ts->nb_playing && (ts->run_state==1)) {
			ts->run_state=0;
			while (ts->run_state!=2) gf_sleep(2);
			if (gf_list_count(m2ts->ts->requested_progs)) {
				ts->file_regulate = 0;
				return TSDemux_DemuxPlay(ts);
			}
		}
		return GF_OK;
	case GF_NET_CHAN_CONFIG:
		pes = M2TS_GetChannel(m2ts, com->base.on_channel);
		/*filter all sections carrying SL data for the app to signal the version number of the section*/
		if (pes && pes->flags & GF_M2TS_ES_IS_SECTION) {
			if (pes->slcfg) gf_free(pes->slcfg);
			pes->slcfg = gf_malloc(sizeof(GF_SLConfig));
			memcpy(pes->slcfg, &com->cfg.sl_config, sizeof(GF_SLConfig));
			com->cfg.use_m2ts_sections = 1;
			pes->flags |= GF_M2TS_ES_SEND_REPEATED_SECTIONS;
		}
		return GF_OK;
	default:
		return GF_OK;
	}
}

static u32 M2TS_RegisterMimeTypes(const GF_InputService * service){
  int i;
  if (service == NULL)
    return 0;
  for (i = 0 ; MIMES[i]; i++)
    gf_term_register_mime_type( service, MIMES[i], "ts m2t dmb", "MPEG-2 TS");
  return i;
}

static void M2TS_GetNetworkType(GF_InputService *plug,M2TSIn *reader)
{
	const char *mob_on;
	const char *mcast_ifce;

	mob_on = gf_modules_get_option((GF_BaseInterface*)plug, "Network", "MobileIPEnabled");
	if(mob_on && !strcmp(mob_on, "yes")){
		reader->ts->MobileIPEnabled = 1;
		reader->ts->network_type = gf_modules_get_option((GF_BaseInterface*)plug, "Network", "MobileIP");
	}

    mcast_ifce = gf_modules_get_option((GF_BaseInterface*)plug, "Network", "DefaultMCastInterface");
	if(mcast_ifce) reader->ts->network_type = gf_strdup(mcast_ifce);
}

GF_InputService *NewM2TSReader()
{
	M2TSIn *reader;
	GF_InputService *plug = gf_malloc(sizeof(GF_InputService));
	memset(plug, 0, sizeof(GF_InputService));
	GF_REGISTER_MODULE_INTERFACE(plug, GF_NET_CLIENT_INTERFACE, "GPAC MPEG-2 TS Reader", "gpac distribution")

	plug->CanHandleURL = M2TS_CanHandleURL;
	plug->CanHandleURLInService = M2TS_CanHandleURLInService;
	plug->ConnectService = M2TS_ConnectService;
	plug->CloseService = M2TS_CloseService;
	plug->GetServiceDescriptor = M2TS_GetServiceDesc;
	plug->ConnectChannel = M2TS_ConnectChannel;
	plug->DisconnectChannel = M2TS_DisconnectChannel;
	plug->ServiceCommand = M2TS_ServiceCommand;
	plug->RegisterMimeTypes = M2TS_RegisterMimeTypes;

	reader = gf_malloc(sizeof(M2TSIn));
	memset(reader, 0, sizeof(M2TSIn));
	plug->priv = reader;
	reader->ts = gf_m2ts_demux_new();
	reader->ts->on_event = M2TS_OnEvent;
	reader->ts->user = reader;
	reader->ts->demux_and_play = 1;
	reader->ts->th = gf_th_new("MPEG-2 TS Demux");

	reader->mx = gf_mx_new("MPEG2 Demux");
	
	return plug;
}

void DeleteM2TSReader(void *ifce)
{
	u32 i, count;
	M2TSIn *m2ts;
	GF_InputService *plug = (GF_InputService *) ifce;
	if (!ifce)
	  return;
	m2ts = plug->priv;
	if (!m2ts)
	  return;
	if( m2ts->ts->requested_progs ){
		count = gf_list_count(m2ts->ts->requested_progs);
		for (i = 0; i < count; i++) {
			M2TSIn_Prog *prog = gf_list_get(m2ts->ts->requested_progs, i);
			gf_free(prog->fragment);
			gf_free(prog);
		}
		gf_list_del(m2ts->ts->requested_progs);
	}
	m2ts->ts->requested_progs = NULL;
	if( m2ts->ts->requested_pids ){
		count = gf_list_count(m2ts->ts->requested_pids);
		for (i = 0; i < count; i++) {
			M2TSIn_Prog *prog = gf_list_get(m2ts->ts->requested_pids, i);
			gf_free(prog);
		}
		gf_list_del(m2ts->ts->requested_pids);
	}
	m2ts->ts->requested_pids = NULL;
	if (m2ts->network_buffer)
		gf_free(m2ts->network_buffer);
	m2ts->network_buffer = NULL;
	m2ts->network_buffer_size = 0;
	m2ts->request_all_pids = 0;
	gf_m2ts_demux_del(m2ts->ts);
	m2ts->ts = NULL;
	gf_mx_del(m2ts->mx);
	m2ts->mx = NULL;
	gf_free(m2ts);
	plug->priv = NULL;
	gf_free(plug);
}

#endif


GF_EXPORT
const u32 *QueryInterfaces()
{
	static u32 si [] = {
#ifndef GPAC_DISABLE_MPEG2TS
		GF_NET_CLIENT_INTERFACE,
#endif
		0
	};
	return si;
}

GF_BaseInterface *LoadInterface(u32 InterfaceType)
{
	switch (InterfaceType) {
#ifndef GPAC_DISABLE_MPEG2TS
	case GF_NET_CLIENT_INTERFACE: return (GF_BaseInterface *) NewM2TSReader();
#endif
	default: return NULL;
	}
}

void ShutdownInterface(GF_BaseInterface *ifce)
{
	switch (ifce->InterfaceType) {
#ifndef GPAC_DISABLE_MPEG2TS
	case GF_NET_CLIENT_INTERFACE:  DeleteM2TSReader(ifce); break;
#endif
	}
}
