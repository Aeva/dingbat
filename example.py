

import sys
import glfw_test

if __name__ == "__main__":
    if sys.version_info[0] < 3:
        print("Requires Python 3")
        exit(1)

    try:
        glfw_test.setup()
        while True:

            # object update code would go here
            
            glfw_test.swap_buffers()
        
    except KeyboardInterrupt:
        glfw_test.teardown()
        exit(0)
