

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

        shader_attrs = dingbat.shader_attrs(prog);
        print(shader_attrs)
                
        dingbat.activate_shader(prog)

        buffer_a = dingbat.create_buffer()
        buffer_b = dingbat.create_buffer()
        dingbat.delete_buffer(buffer_a)

        clip_space_triangle = [
            0.0, 0.5, 0.0,
            -0.5, -0.5, 0.0,
            0.5, -0.5, 0.0
        ]
        
        # https://docs.python.org/3.7/library/array.html
        triangle_data = array.array('f', clip_space_triangle)
        dingbat.fill_buffer(buffer_b, triangle_data)
        
        last = time.monotonic()
        while True:
            now = time.monotonic()
            delta = now - last
            last = now
            #print ("delta: %s" % str(1/delta))
            
            dingbat.naive_draw(buffer_b, 0, len(clip_space_triangle))
            
            dingbat.swap_buffers()
        
    except KeyboardInterrupt:
        dingbat.teardown()
        exit(0)
