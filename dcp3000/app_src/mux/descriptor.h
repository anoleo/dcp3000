 /*****************************************************************************
 *descriptor.h : dvb mpeg-2 including each descriptor structure description
 *----------------------------------------------------------------------------
 * (c) 2004-2005 mx3000
 * Authors: Davy.Lee<pbi-lz@pbi-china.com>
 *
 * change log :
 *		2004-6-29 10:07  v1.0	start to write 
 *		2004-7-1  16:24  v1.0	add point link to all of si structure
 *
 *----------------------------------------------------------------------------
 *
 *****************************************************************************/
 #ifndef _DESCRIPTOR_H_
 #define _DESCRIPTOR_H_

#define FORMAT_IDENTIFIER			0x05
 #define CA_DESTAG					0x09
 #define ISO_639_LANGUAGE_DESTAG	0x0a 
 #define NETWORK_NAME_DESTAG	0x40
 #define SEVICE_LIST_DESTAG		0x41
 #define STUFFING_DESTAG		0x42
 #define SATALITE_DELIVERY_SYSTEN_DESTAG	0x43
 #define CABLE_DELIVERY_SYSTEN_DESTAG	0x44
 #define BOUQUET_NAME_DESTAG	0x47
 #define SERVICE_DESTAG			0x48
 #define COUNTRY_AVAILABILITY_DESTAG	0x49
 #define LINKAGE_DESTAG			0x4a
 #define NVOD_REFRENCE_DESTAG	0x4b
 #define TIME_SHIFTED_SERVICE_DESTAG	0x4c
 #define SHORT_EVENT_DESTAG	0x4d
 #define EXTENDED_EVENT_DESTAG 0x4e
 #define TIME_SHIFTED_EVENT_DESTAG	0x4f
 #define COMPONENT_DESTAG		0x50
 #define MOSAIC_DESTAG			0x51
 #define STREM_IDENTIFIER_DESTAG	0x52
 #define CA_IDENTIFIER_DESTAG	0x53
 #define CONTENT_DESTAG			0x54
 #define PARENTAL_RATING_DESTAG 	0x55
 #define TELETEXT_DESTAG		0x56
 #define TELEPHONE_DESTAG		0x57
 #define LOCAL_TIME_OFFSET_DESTAG	0x58
 #define SUBTITLING_DESTAG		0x59
 #define TERRESTRIAL_DELIVERY_SYSTEM_DESTAG	0x5a
 #define MULTILINGUAL_NETWORK_NAME_DESTAG	0x5b
 #define MULTILINGUAL_BOUQUET_NAME_DESTAG	0x5c
 #define MULTILINGUAL_SERVICE_NAME_DESTAG	0x5d
 #define MULTILINGUAL_COMPONENT_DESTAG		0x5e
 #define PRIVATE_DATA_SPECIFIER_DESTAG			0x5f
 #define SERVICE_MOVE_DESTAG	0x60
 #define SHORT_SMOOTHING_BUFFER_DESTAG	0x61
 #define AC3_DESTAG				0x6a
 #define VBI_DATA_DESTAG			0xc3 //0x45; don't use 0x45 as tag
 #define FORBIDDEN				0xff
  
 
/* video stream descriptor 
 * descriptor_tag is x02 */
typedef struct  _descVidStream_t {
    u_int      multiple_frame_rate_flag;
    u_int      frame_rate_code;
    u_int      MPEG_2_flag;
    u_int      constrained_parameter_flag;
    u_int      still_picture_flag;

    // if MPEG_2_flag == 1

    u_int	profile_and_level_indication;
    u_int	chroma_format;
    u_int	frame_rate_extension_flag;
    u_int	reserved_1;

} descVidStream_t;

/* audio stream descriptor 
 * descriptor_tag is x03 */
typedef struct  _descAudioStream_t {
    u_int      free_format_flag;
    u_int      ID;
    u_int      layer;
    u_int      variable_rate_audio_indicator;  // ?
    u_int      reserved_1;

} descAudioStream_t;

/* hierarchy descriptor 
 * descriptor_tag is x04 */
 typedef struct  _descHierarchy_t {
    u_int      reserved_1;
    u_int      hierarchy_type;
    u_int      reserved_2;
    u_int      hierarchy_layer_index;
    u_int      reserved_3;
    u_int      hierarchy_embedded_layer;
    u_int      reserved_4;
    u_int      hierarchy_priority;

 } descHierarchy_t;

/* registration descriptor 
 * descriptor_tag is x05 */
 typedef struct  _descRegistration_t {
    u_long      format_identifier;
    // N   add. info

 } descRegistration_t;

/* data stream alignment descriptor 
 * descriptor_tag is x06 */
typedef struct  _descDataStreamAlignment_t {
    u_int      alignment_type;

 } descDataStreamAlignment_t;

/* target background grid descriptor 
 * descriptor_tag is x07 */
 typedef struct  _descTBGG_t {
    u_int      horizontal_size;
    u_int      vertical_size;
    u_int      pel_aspect_ratio;

 } descTBGG_t;

/* video windows descriptor 
 * descriptor_tag is x08 */
 typedef struct  _descVidWin_t {
    u_int      horizontal_offset;
    u_int      vertical_offset;
    u_int      window_priority;

 } descVidWin_t;

/* ca descriptor 
 * descriptor_tag is x09 */
 typedef struct  _descCA_t {		
    u_short	CA_system_ID;
    u_short	CA_PID;
    //u_char	reserved;

    // private data bytes

} descCA_t;

/* iso 639 language descriptor   //??
 * descriptor_tag is x0a */
 typedef struct  _descISO639_t {
    // N ISO639 desc

} descISO639_t;

 typedef struct  _descISO639List_t {
    u_char      ISO_639_language_code[8][3];
    u_int      audio_type;
 } descISO639List_t;

/* system clock descriptor 
 * descriptor_tag is x0b */
 typedef struct  _descSysClock_t {
    u_int      external_clock_reference_indicator;
    u_int      reserved_1;
    u_int      clock_accuracy_integer;
    u_int      clock_accuracy_exponent;
    u_int      reserved_2;

} descSysClock_t;

/* multiplex buffer utilization descriptor 
 * descriptor_tag is x0c */
  typedef struct  _descMBU_t {
    u_int      mdv_valid_flag;
    u_int      multiplex_delay_variation;
    u_int      multiplex_strategy;
    u_int      reserved_1;

 } descMBU_t;

/* copyright descriptor 
 * descriptor_tag is x0d */
typedef struct  _descCopyright_t {
    u_long     copyright_identifier;

    // add. info

 } descCopyright_t;

/* maximum bitrate descriptor 
 * descriptor_tag is x0e */
 typedef struct  _descMaxBitrate_t {
    u_int      reserved_1;
    u_long     maximum_bitrate;

 } descMaxBitrate_t;

/* private data indicator descriptor 
 * descriptor_tag is x0f  */
 typedef struct  _descPrivateDataIndicator_t {
    u_int      private_data_indicator;
 } descPrivateDataIndicator_t;
 
/* smoothing Buffer  descriptor 
 * descriptor_tag is x010 */
  typedef struct  _descSmoothingBuf_t {
    u_int      reserved_1;
    u_long     sb_leak_rate;
    u_int      reserved_2;
    u_long     sb_size;

} descSmoothingBuf_t;

/* std descriptor 
 * descriptor_tag is x011 */
  typedef struct  _descSTD_t {
    u_int      reserved_1;
    u_int      leak_valid_flag;

} descSTD_t;

/* ibp descriptor 
 * descriptor_tag is x012 */
 typedef struct  _descIBP_t {
    u_int      closed_gop_flag;
    u_int      identical_gop_flag;
    u_int      max_gop_length;

 } descIBP_t;

/* net name descriptor 
 * descriptor_tag is x040 */
 
 typedef struct  _descNND_t {
    u_char     *network_name;   // with controls

 } descNND_t;

/* service list descriptor 
 * descriptor_tag is x041 */
 typedef struct  _descServList_t {
    // N ... Service ID & Types

 } descServList_t;

 typedef struct _descServList2_t {
    u_int      service_id;
    u_int      service_type;
 } descServList2_t;
 
 /* stuffing descriptor 
 * descriptor_tag is x042 */
 typedef struct  _descStuffing_t {
    // N ... stuffing data

} descStuffing_t;

/* Satellite delivery system descriptor 
 * descriptor_tag is x043 */
 typedef struct  _descSDS_t {
    u_long     frequency;
    u_int      orbital_position;
    u_int      west_east_flag;
    u_int      polarisation;
    u_int      modulation;
    u_long     symbol_rate;
    u_int      FEC_inner;

} descSDS_t;

/* Cable delivery system descriptor
 * descriptor_tag is x044 */
  typedef struct  _descCDS_t {
    u_long     frequency;
    u_int      FEC_outer;
    u_int      reserved_1;
    u_int      modulation;
    u_long     symbol_rate;
    u_int      FEC_inner;

} descCDS_t;

/* VBI Data  descriptor 
 * descriptor_tag is x045 */
  typedef struct  _descVBIData_t {
    // N ... DataList 1
 } descVBIData_t;

 typedef struct  _descVBIData2_t {
    u_int      data_service_id;		
    u_int      data_service_descriptor_length;		

    //    N ... DataList 3
    // or N ... reserved bytes
 } descVBIData2_t;

 typedef struct  _descVBIData3_t {
    u_int      reserved_1;		
    u_int      field_parity;		
    u_int      line_offset;		
 } descVBIData3_t;

/* VBI teletext descriptor 
 * descriptor_tag is x046 */
 typedef struct  _descVBITeletext_t {
    // N TeleTextList desc

 } descVBITeletext_t;

 typedef struct  _descVBITeletextList_t {
    u_char      ISO_639_language_code[3];
    u_int      teletext_type;
    u_int      teletext_magazine_number;
    u_int      teletext_page_number;
 } descVBITeletextList_t;
 
/* Bouquet Name  descriptor 
 * descriptor_tag is x047 */
  typedef struct  _descBouquetName_t {
    // N   ... char name

 } descBouquetName_t;

/* Service descriptor 
 * descriptor_tag is x048 */
  typedef struct  _descService_t {
    u_char    service_type;
    u_char    service_provider_name_length;
    u_char   service_provider_name[128];
    u_char    service_name_length;
    u_char   service_name[128];

} descService_t;

/* Country Availibility  descriptor
 * descriptor_tag is x049 */
 typedef struct  _descCountryAvail_t {
    u_int      country_availability_flag;
    u_int      reserved_1;

    //  N   countrycodes[3]

} descCountryAvail_t;

/* Linkage descriptor
 * descriptor_tag is x04a */
  typedef struct  _descLinkage_t {
    u_int      transport_stream_id;
    u_int      original_network_id;
    u_int      service_id;
    u_int      linkage_type;

    // if linkage_type == 8
    // the following field are conditional!!
    u_int      handover_type;
    u_int      reserved_1; 
    u_int      origin_type;
    u_int      network_id;
    u_int      initial_service_id;

 } descLinkage_t;

/* NVOD Reference descriptor
 * descriptor_tag is x04b */
  typedef struct  _descNVODRef_t {
    // N ...  Ref2
 } descNVODRef_t;

 typedef struct  _descNVODRef2_t {
    u_int      transport_stream_id;
    u_int      original_network_id;
    u_int      service_id;
 } descNVODRef2_t;

/* Time Shifted Service descriptor
 * descriptor_tag is x04c */
  typedef struct  _descTimShiftServ_t {
    u_int      reference_service_id;

} descTimShiftServ_t;

/* Short Event descriptor
 * descriptor_tag is x04d */
  typedef struct  _descShortEvent_t {
    u_char     ISO639_2_language_code[3];
    u_int      event_name_length;

    // N   char event_name

    u_int      text_length;

    // N2  char  text char

} descShortEvent_t;

/* Extended Event descriptor
 * descriptor_tag is x04e */
  typedef struct  _descExtendedEvent_t {
    u_int      descriptor_number;
    u_int      last_descriptor_number;
    u_char     ISO639_2_language_code[3];
    u_int      length_of_items;

    // N   Ext. Events List

    u_int      text_length;
    // N4  char  text char
 } descExtendedEvent_t;

 typedef struct  _descExtendedEvent2_t {
    u_int      item_description_length;
    //  N2   descriptors
    u_int      item_length;
    //  N3   chars
 } descExtendedEvent2_t;

/* Time Shifted Event descriptor
 * descriptor_tag is x04f */
  typedef struct  _descTimeShiftedEvent_t {
    u_int      reference_service_id;
    u_int      reference_event_id;

} descTimeShiftedEvent_t;

/* Component descriptor
 * descriptor_tag is x050 */
  typedef struct  _descComponent_t {
    u_int      reserved_1;
    u_int      stream_content;
    u_int      component_type;
    u_int      component_tag;
    u_char     ISO639_2_language_code[3];

    // N2  char Text

} descComponent_t;

/* Mosaic descriptor
 * descriptor_tag is x051 */
  typedef struct  _descMosaic_t {
    u_int      mosaic_entry_point;
    u_int      number_of_horizontal_elementary_cells;
    u_int      reserved_1;
    u_int      number_of_vertical_elementary_cells;

    // N    desc Mosaic2

 } descMosaic_t;

 typedef struct  _descMosaic2_t {
    u_int      logical_cell_id;
    u_int      reserved_1;
    u_int      logical_cell_presentation_info;
    u_int      elementary_cell_field_length;

    // N2   desc Mosaic3

    u_int      cell_linkage_info;
    //  conditional data !! (cell_linkage_info)
    u_int   bouquet_id;
    u_int   original_network_id;
    u_int   transport_stream_id;
    u_int   service_id;
    u_int   event_id;
 } descMosaic2_t;

 typedef struct  _descMosaic3_t {
    u_int      reserved_1;
    u_int      elementary_cell_id;
 } descMosaic3_t;
 
/* Stream Identifier descriptor
 * descriptor_tag is x052 */
 typedef struct  _descStreamIdent_t {
    u_int      component_tag;		
 } descStreamIdent_t;
 
/* CA Identifier descriptor
 * descriptor_tag is x053 */
  typedef struct  _descCAIdent_t {

    // N   CA_SysIDs

} descCAIdent_t;

/* Content descriptor
 * descriptor_tag is x054 */
  typedef struct  _descContent_t {
   		
 } descContent_t;

 typedef struct  _descContent2_t {
    u_int      content_nibble_level_1;
    u_int      content_nibble_level_2;
    u_int      user_nibble_1;
    u_int      user_nibble_2;
 } descContent2_t;
 
/* Parental Rating descriptor
 * descriptor_tag is x055 */
  typedef struct  _descParentalRating_t {
		
 } descParentalRating_t;

 typedef struct  _descParentalRating2_t {
    u_char     country_code[3];
    u_int      rating;		
 } descParentalRating2_t;
 
/* Teletext descriptor
 * descriptor_tag is x056 */
  typedef struct  _descTeletext_t {

    // N TeleTextList desc

 } descTeletext_t;

 typedef struct  _descTeletextList_t {
    u_char      ISO_639_language_code[3];
    u_int      teletext_type;
    u_int      teletext_magazine_number;
    u_int      teletext_page_number;
 } descTeletextList_t;

/* Telephone descriptor
 * descriptor_tag is x057 */
  typedef struct  _descTelephone_t {
    u_int      reserved_1;
    // $$$ ToDO

 } descTelephone_t;

/* Local Time Offset descriptor
 * descriptor_tag is x058 */
  typedef struct  _descLocalTimeOffset_t {

    // N  Descriptor
 } descLocalTimeOffset_t;

 typedef struct  _descLocalTimeOffset2_t {
   u_char        country_code[3];
   u_int         country_region_id;
   u_int         reserved_1;
   u_int         local_time_offset_polarity;
   u_int         local_time_offset;
   u_int         time_of_change_MJD;
   u_int         time_of_change_UTC;
   u_int         next_time_offset;
 } descLocalTimeOffset2_t;
 
/* Subtitling descriptor
 * descriptor_tag is x059 */
  typedef struct  _descSubTitling_t {
   
 } descSubTitling_t;

 typedef struct  _descSubTitling2_t {
    u_char     ISO_639_language_code[3];
    u_int      subtitling_type;
    u_int      composition_page_id;
    u_int      ancillary_page_id;
 } descSubTitling2_t;
 
/* Terrestrial delivery system descriptor
 * descriptor_tag is x05a */ 
  typedef struct  _descTDS_t {
     u_long     centre_frequency;
    u_int      bandwidth;
    u_int      reserved_1;
    u_int      constellation;
    u_long     hierarchy_information;
    u_int      code_rate_HP_stream;
    u_int      code_rate_LP_stream;
    u_int      guard_interval;
    u_int      transmission_mode; 
    u_int      other_frequency_flag;
    u_int      reserved_2;

 } descTDS_t;
 
/* Multilingual Network Name descriptor
 * descriptor_tag is x05b */ 
  typedef struct  _descMultiNetName_t {
     //  N .. List2

 } descMultiNetName_t;

 typedef struct  _descMultiNetName2_t {
    u_char     ISO639_2_language_code[3];
    u_int      network_name_length;

    //  N2 ..  char

 } descMultiNetName2_t;

/* Multilingual Bouquet Name descriptor
 * descriptor_tag is 0x5c */ 
  typedef struct  _descMultiBouqName_t {

    //  N .. List2

 } descMultiBouqName_t;

 typedef struct  _descMultiBouqName2_t {
    u_char     ISO639_2_language_code[3];
    u_int      bouquet_name_length;

    //  N2 ..  char

} descMultiBouqName2_t;

/* Multilingual Service Name descriptor
 * descriptor_tag is 0x5d */ 
  typedef struct  _descMultiServiceName_t {
 
    //  N .. List2

 } descMultiServiceName_t;

 typedef struct  _descMultiServiceName2_t {
    u_char     ISO639_2_language_code[3];
    u_int      service_provider_name_length;

    //  N2 ..  char

    u_int      service_name_length;

    //  N3 ..  char

} descMultiServiceName2_t;

/* Multilingual Component descriptor
 * descriptor_tag is 0x5e */
  typedef struct  _descMultiComponent_t {
 
    u_int      component_tag;

    //  N .. List2

 } descMultiComponent_t;

 typedef struct  _descMultiComponent2_t {
    u_char     ISO639_2_language_code[3];
    u_int      text_description_length;

    //  N2 ..  char

} descMultiComponent2_t;

/* Private Data Specifier descriptor
 * descriptor_tag is 0x5f */
 typedef struct  _descPrivDataSpec_t {

    u_long     private_data_specifier;

} descPrivDataSpec_t;

/* Service Move descriptor
 * descriptor_tag is 0x60 */
 typedef struct  _descServMove_t {
    u_int      new_original_network_id;
    u_int      new_transport_stream_id;
    u_int      new_service_id;

} descServMove_t;

/* Short Smoothing Buffer descriptor
 * descriptor_tag is 0x61 */
  typedef struct  _descSSBuf_t {
    u_int      sb_size;
    u_int      sb_leak_rate;

 } descSSBuf_t;

/* Frequency List descriptor
 * descriptor_tag is 0x62 */
 typedef struct  _descFreqList_t {
    u_int      reserved_1;
    u_int      coding_type;
 } descFreqList_t;

 typedef struct  _descFreqList2_t {
    u_long     centre_frequency;
 } descFreqList2_t;
 
/* Partial Transport Stream descriptor
 * descriptor_tag is 0x63 */
 typedef struct  _descPartTranspStream_t {
    u_int      reserved_1;
    u_long     peak_rate;
    u_int      reserved_2;
    u_long     minimum_overall_smoothing_rate;
    u_int      reserved_3;
    u_int      maximum_overall_smoothing_buffer;

} descPartTranspStream_t;

/* DataBroadcast descriptor
 * descriptor_tag is 0x64 */
 typedef struct  _descDataBroadcast_t {
    u_int      data_broadcast_id;
    u_int      component_tag;
    u_int      selector_length;

    // N   bytes

    u_char     ISO639_2_language_code[3];
    u_int      text_length;

    // N2  char 

} descDataBroadcast_t;

/* CA System descriptor
 * descriptor_tag is 0x65 */
//typedef descCaSsytem descANY;

/* Data Broadcast ID descriptor
 * descriptor_tag is 0x66 */
 typedef struct  _descDataBroadcastID_t {
    u_int      data_broadcast_id;

    //  N ... id_selector bytes
 } descDataBroadcastID_t;

/* Transport Stream descriptor
 * descriptor_tag is 0x67 */
 typedef struct  _descTransportStream_t {
    //   N ... bytes
 } descTransportStream_t;
 
/* DSNG descriptor
 * descriptor_tag is 0x68 */
 typedef struct  _descDSNG_t {

    //  N ... bytes
 } descDSNG_t;
 
/* PDC descriptor
 * descriptor_tag is 0x69 */
 typedef struct  _descPDC_t {
    u_int      reserved_1;
    u_long     programme_identification_label;
    // ... splits in
    u_int     day;
    u_int     month;
    u_int     hour;
    u_int     minute;

} descPDC_t;

/* AC-3 descriptor
 * descriptor_tag is 0x6a */
 typedef struct  _descAC3_t {
    u_int      AC3_type_flag;
    u_int      bsid_flag;
    u_int      mainid_flag;
    u_int      asvc_flag;
    u_int      reserved_1;

    // conditional vars
    u_int      AC3_type;
    u_int      bsid_type;
    u_int      mainid_type;
    u_int      asvc_type;

    // N ...  bytes add info

} descAC3_t;

/* Ancillary Data descriptor
 * descriptor_tag is 0x6b */
 typedef struct  _descAncillaryData_t {
    u_int      ancillary_data_identifier;

 } descAncillaryData_t;

/* Cell List descriptor
 * descriptor_tag is 0x6c */
 
 //typedef descCellList_t descANY;
 
/* Cell Frequency Link descriptor
 * descriptor_tag is 0x6d */
 //typedef descCellFreqList_t descANY;
 
/* Announcement Support descriptor
 * descriptor_tag is 0x6e */
 typedef struct  _descAnnouncementSupport_t {
    u_int      announcement_support_indicator;
    // N .. Announcement 2

 } descAnnouncementSupport_t;

 typedef struct  _descAnnouncementSupport2_t {
    u_int      announcement_type;
    u_int      reserved_1;
    u_int      reference_type;
    // conditional data
    u_int      original_network_id;
    u_int      transport_stream_id;
    u_int      service_id;
    u_int      component_tag;

} descAnnouncementSupport2_t;

/* descriptor private data */
 typedef union   _descriptor_info_t {
    descVidStream_t			descVidStream;
    descAudioStream_t			descAudioStream;
    descHierarchy_t			descHierarchy;
    descRegistration_t			descRegistration;
    descDataStreamAlignment_t		descDataStreamAlignment;
    descTBGG_t				descTBGG;
    descVidWin_t			descVidWin;
    descCA_t				descCA;
    descISO639_t			descISO639;
    descISO639List_t			descISO639List;
    descSysClock_t			descSysClock;
    descMBU_t				descMBU;
    descCopyright_t			descCopyright;
    descMaxBitrate_t			descMaxBitrate;
    descPrivateDataIndicator_t		descPrivateDataIndicator;
    descSmoothingBuf_t			descSmoothingBuf;
    descSTD_t				descSTD;
    descIBP_t				descIBP;
    descNND_t				descNND;
    descServList_t			descServList;
    descServList2_t			descServList2;
    descStuffing_t			descStuffing;
    descSDS_t				descSDS;
    descCDS_t				descCDS;
    descVBIData_t			descVBIData;
    descVBIData2_t			descVBIData2;
    descVBIData3_t			descVBIData3;
    descVBITeletext_t			descVBITeletext;
    descVBITeletextList_t		descVBITeletextList;
    descBouquetName_t			descBouquetName;
    descService_t			descService;
    descCountryAvail_t			descCountryAvail;
    descLinkage_t			descLinkage;
    descNVODRef_t			descNVODRef;
    descNVODRef2_t			descNVODRef2;
    descTimShiftServ_t			descTimShiftServ;
    descShortEvent_t			descShortEvent;
    descExtendedEvent_t			descExtendedEvent;
    descExtendedEvent2_t		descExtendedEvent2;
    descTimeShiftedEvent_t		descTimeShiftedEvent;
    descComponent_t			descComponent;
    descMosaic_t			descMosaic;
    descMosaic2_t			descMosaic2;
    descMosaic3_t			descMosaic3;
    descStreamIdent_t			descStreamIdent;
    descCAIdent_t			descCAIdent;
    descContent_t			descContent;
    descContent2_t			descContent2;
    descParentalRating_t		descParentalRating;
    descParentalRating2_t		descParentalRating2;
    descTeletext_t			descTeletext;
    descTeletextList_t			descTeletextList;
    descTelephone_t			descTelephone;
    descLocalTimeOffset_t		descLocalTimeOffset;
    descLocalTimeOffset2_t		descLocalTimeOffset2;
    descSubTitling_t			descSubTitling;
    descSubTitling2_t			descSubTitling2;
    descTDS_t				descTDS;
    descMultiNetName_t			descMultiNetName;
    descMultiNetName2_t			descMultiNetName2;
    descMultiBouqName_t			descMultiBouqName;
    descMultiBouqName2_t		descMultiBouqName2;
    descMultiServiceName_t		descMultiServiceName;
    descMultiServiceName2_t		descMultiServiceName2;
    descMultiComponent_t		descMultiComponent;
    descMultiComponent2_t		descMultiComponent2;
    descPrivDataSpec_t			descPrivDataSpec;
    descServMove_t			descServMove;
    descSSBuf_t				descSSBuf;
    descFreqList_t			descFreqList;
    descFreqList2_t			descFreqList2;
    descPartTranspStream_t		descPartTranspStream;
    descDataBroadcast_t			descDataBroadcast;
    descDataBroadcastID_t		descDataBroadcastID;
    descTransportStream_t		descTransportStream;
    descDSNG_t				descDSNG;
    descPDC_t				descPDC;
    descAC3_t				descAC3;
    descAncillaryData_t			descAncillaryData;
    descAnnouncementSupport_t		descAnnouncementSupport;
    descAnnouncementSupport2_t		descAnnouncementSupport2;    
  } descriptor_info_t;

/* Any  descriptor  (Basic Descriptor output) */
typedef struct  _descriptor_t {
    	u_char      desc_tag;
    	u_char      desc_len;		

    	//u_char   desc_data[256];
		u_char   *desc_data;
        
    	// private data bytes
    	u_char	   parse_desc_flag;
	descriptor_info_t   desc_info;
	struct _descriptor_t *link;
} descriptor_t;

 #endif
