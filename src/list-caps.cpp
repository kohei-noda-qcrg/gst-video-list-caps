#include <gst/gst.h>
#include <gst/video/video.h>

#include "gstutil/auto-gst-object.hpp"
#include "gstutil/pipeline-helper.hpp"
#include "macros/assert.hpp"
#include "macros/autoptr.hpp"
#include "macros/unwrap.hpp"

namespace {
declare_autoptr(GstCaps, GstCaps, gst_caps_unref);
}

auto print_caps(const GstCaps* caps) -> void {
    int num_structures = gst_caps_get_size(caps);
    print("  Number of structures: ", num_structures);
    for(int i = 0; i < num_structures; i++) {
        const auto* structure = gst_caps_get_structure(caps, i);
        const auto* name      = gst_structure_get_name(structure);

        print("  Format: ", name);

        gint width, height;
        if(gst_structure_get_int(structure, "width", &width) &&
           gst_structure_get_int(structure, "height", &height)) {
            print("    Resolution: ", width, "x", height);
        }
    }
}

auto run(int argc, char* argv[]) -> bool {
    assert_b(argc == 2, "usage: main [device-path]");

    gst_init(&argc, &argv);

    const auto device_path = std::string_view(argv[1]);

    auto pipeline = AutoGstObject(gst_pipeline_new(NULL));
    assert_b(pipeline.get() != NULL);
#if defined(_WIN32)
    const auto videosrc_name        = "ksvideosrc";
    const auto videosrc_device_prop = "device-path";
#else
    const auto videosrc_name        = "v4l2src";
    const auto videosrc_device_prop = "device";
#endif
    unwrap_pb_mut(videosrc, add_new_element_to_pipeine(pipeline.get(), videosrc_name));
    g_object_set(&videosrc, videosrc_device_prop, device_path.data(), NULL);
    unwrap_pb_mut(sink, add_new_element_to_pipeine(pipeline.get(), "fakesink"));
    assert_b(gst_element_link_pads(&videosrc, NULL, &sink, NULL) == TRUE);

    gst_element_set_state(pipeline.get(), GST_STATE_READY);

    auto* pad = gst_element_get_static_pad(&videosrc, "src");
    // auto* caps = gst_pad_query_caps(pad, NULL);
    auto caps = AutoGstCaps(gst_pad_query_caps(pad, NULL));
    if(caps) {
        print("Source capabilities:");
        print_caps(caps.get());
    } else {
        warn("Failed to query capabilities from pad.");
    }

    gst_element_set_state(pipeline.get(), GST_STATE_NULL);
    return 0;
}

auto main(int argc, char* argv[]) -> int {
    return run(argc, argv) ? 0 : 1;
}