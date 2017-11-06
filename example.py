

import sys
import time
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
        
        last = time.monotonic()
        while True:
            now = time.monotonic()
            delta = now - last
            last = now
            print ("delta: %s" % str(1/delta))
            
            # object update code would go here
            
            glfw_test.swap_buffers()
        
    except KeyboardInterrupt:
        glfw_test.teardown()
        exit(0)
