

import sys
import time
import math
import array
import dingbat

if __name__ == "__main__":
    if sys.version_info[0] < 3:
        print("Requires Python 3")
        exit(1)

    # test_handle = dingbat.create_test_handle();
    # print("User deletes managed object")
    # del test_handle
    
    try:
        dingbat.setup()
        with open("test.vert", "r") as vert_file:
            with open("test.frag", "r") as frag_file:
                prog = dingbat.build_shader(vert_file.read(), frag_file.read())
        assert(prog)
        attrs = dingbat.shader_attrs(prog)
                
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


        uniform_buffer = dingbat.create_buffer()
        uniform_block = dingbat.shader_uniform_blocks(prog)["SomeBlock"]
        dingbat.fill_uniform_block(uniform_buffer, uniform_block, 0.0)
        print(attrs, uniform_block)
        
        # https://docs.python.org/3.7/library/array.html
        dingbat.fill_buffer(position_buffer, array.array('f', clip_space_triangle))
        dingbat.fill_buffer(color_buffer, array.array('f', vertex_colors))

        technicolor_rainbow_triangle_of_justice = [
            dingbat.bind_attr_buffer(position_buffer, attrs["VertexPosition"], 3),
            dingbat.bind_attr_buffer(color_buffer, attrs["VertexColor"], 3),
            dingbat.bind_uniform_buffer(uniform_buffer, uniform_block),
            dingbat.bind_draw_arrays(0, 9),
        ]
        
        last = time.monotonic()
        while True:
            now = time.monotonic()
            delta = now - last
            last = now
            print ("delta: %s" % str(1/delta))

            offset = math.sin(now * 1.5) * 0.6
            dingbat.fill_uniform_block(uniform_buffer, uniform_block, offset)

            dingbat.clear()
            dingbat.batch_draw(*technicolor_rainbow_triangle_of_justice)
            dingbat.swap_buffers()
        
    except KeyboardInterrupt:
        dingbat.teardown()
        exit(0)
