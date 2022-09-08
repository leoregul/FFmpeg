#ifndef AVFORMAT_SCTE_35_H
#define AVFORMAT_SCTE_35_H

#include "libavutil/bprint.h"

#if defined(_WIN32)
    #define AV_EXPORT __declspec(dllexport)
    #define AV_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
    #define AV_EXPORT __attribute__((visibility("default")))
    #define AV_IMPORT
#else
    #define AV_EXPORT
    #define AV_IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif

#if CONFIG_SHARED && !defined(BUILDING_avformat)
#   define av_export_avformat AV_IMPORT
#else
#   define av_export_avformat AV_EXPORT
#endif

struct scte35_event {
    /* ID given for each separate event */
    int32_t id;
    /* pts specify time when event starts */
    uint64_t in_pts;
    uint64_t nearest_in_pts;
    /* pts specify when events end */
    uint64_t out_pts;
    /* duration of the event */
    int64_t duration;
    int64_t start_pos;
    int running;
    int ref_count;
    /* to traverse the list of events */
    struct scte35_event *next;
    struct scte35_event *prev;
};

enum scte35_event_state {
    /* NO event */
    EVENT_NONE,
    /* Commercials need to end */
    EVENT_IN,
    /* Commercials can start from here */
    EVENT_OUT,
    /* commercial can continue */
    EVENT_OUT_CONT,
};

struct scte35_interface {
    /* contain all  the events */
    struct scte35_event *event_list;
    /* state of current event */
    enum scte35_event_state event_state;
    /* time base of pts used in parser */
    AVRational timebase;
    struct scte35_event *current_event;
    /* saved previous state to correctly transition the event state */
    enum scte35_event_state prev_event_state;
    //TODO use AV_BASE64_SIZE to dynamically allocate the array
    char pkt_base64[1024];
    /* keep context of its parent for log */
    void *parent;
    /* general purpose str */
    AVBPrint avbstr;

    void (*update_video_pts)(struct scte35_interface *iface, uint64_t pts);
    struct scte35_event* (*update_event_state)(struct scte35_interface *iface);
    char* (*get_hls_string)(struct scte35_interface *iface, struct scte35_event *event,
               const char *adv_filename, int out_state, int seg_count, int64_t pos);

    void (*unref_scte35_event)(struct scte35_event **event);
    void (*ref_scte35_event)(struct scte35_event *event);
};

extern av_export_avformat int ff_parse_scte35_pkt(struct scte35_interface *iface, const AVPacket *avpkt);

extern av_export_avformat struct scte35_interface* ff_alloc_scte35_parser(void *parent, AVRational timebase);
extern av_export_avformat void ff_delete_scte35_parser(struct scte35_interface* iface);

#endif /* AVFORMAT_SCTE_35_H */
