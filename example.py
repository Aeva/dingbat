

import sys
import time
import array
import dingbat

if __name__ == "__main__":
    if sys.version_info[0] < 3:
        print("Requires Python 3")
        exit(1)

    try:
        dingbat.setup()
        with open("test.vert", "r") as vert_file:
            with open("test.frag", "r") as frag_file:
                prog = dingbat.build_shader(vert_file.read(), frag_file.read())
        assert(prog)
                
        dingbat.activate_shader(prog)

        position_buffer = dingbat.create_buffer()
        color_buffer = dingbat.create_buffer()

        clip_space_triangle = [
            0.0, 0.5, 0.0,
            -0.5, -0.5, 0.0,
            0.5, -0.5, 0.0
        ]

        vertex_colors = [
            1.0, 1.0, 0.0,
            0.0, 1.0, 1.0,
            1.0, 0.0, 1.0
        ]
        
        # https://docs.python.org/3.7/library/array.html
        dingbat.fill_buffer(position_buffer, array.array('f', clip_space_triangle))
        dingbat.fill_buffer(color_buffer, array.array('f', vertex_colors))

        technicolor_rainbow_triangle_of_justice = [
            dingbat.bind_attr_buffer(position_buffer, 0, 3),
            dingbat.bind_attr_buffer(color_buffer, 1, 3),
            dingbat.bind_draw_arrays(0, 9),
        ]
        
        last = time.monotonic()
        while True:
            now = time.monotonic()
            delta = now - last
            last = now
            #print ("delta: %s" % str(1/delta))

            dingbat.batch_draw(*technicolor_rainbow_triangle_of_justice)
            dingbat.swap_buffers()
        
    except KeyboardInterrupt:
        dingbat.teardown()
        exit(0)
