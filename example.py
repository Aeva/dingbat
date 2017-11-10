

import sys
import time
import array
import glfw_test

if __name__ == "__main__":
    if sys.version_info[0] < 3:
        print("Requires Python 3")
        exit(1)

    try:
        glfw_test.setup()
        with open("test.vert", "r") as vert_file:
            with open("test.frag", "r") as frag_file:
                prog = glfw_test.build_shader(vert_file.read(), frag_file.read())
        assert(prog)
        glfw_test.activate_shader(prog)

        buffer_a = glfw_test.create_buffer()
        buffer_b = glfw_test.create_buffer()
        glfw_test.delete_buffer(buffer_a)

        clip_space_triangle = [
            0.0, 0.5, 0.0,
            -0.5, -0.5, 0.0,
            0.5, -0.5, 0.0
        ]
        
        # https://docs.python.org/3.7/library/array.html
        triangle_data = array.array('f', clip_space_triangle)
        glfw_test.fill_buffer(buffer_b, triangle_data)
        
        last = time.monotonic()
        while True:
            now = time.monotonic()
            delta = now - last
            last = now
            print ("delta: %s" % str(1/delta))
            
            glfw_test.naive_draw(buffer_b, 0, len(clip_space_triangle))
            
            glfw_test.swap_buffers()
        
    except KeyboardInterrupt:
        glfw_test.teardown()
        exit(0)
