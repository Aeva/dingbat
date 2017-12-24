

import sys
import time
import math
import array
import dingbat




def read_obj(path):
    vertices = []
    model_data = []
    with open(path, "r") as obj_file:
        for line in obj_file:
            params = line.strip().split(" ")
            if not params:
                continue
            command, params = params[0], params[1:]
            if command == "v":
                vertices.append(list(map(float, params)))
                continue
            if command == "f":
                for param in params:
                    index = int(param)-1
                    model_data += vertices[index]
    return tuple(model_data)




class DemoDrawable:
    def __init__(self, shader, vertices, colors):
        position_buffer = dingbat.create_buffer()
        color_buffer = dingbat.create_buffer()
        attrs = dingbat.shader_attrs(shader)

        # https://docs.python.org/3.7/library/array.html
        dingbat.fill_buffer(position_buffer, array.array('f', vertices))

        # We only need to hold on to the handle for the buffer objects
        # we intend to update.  The bindings below will ensure that
        # the ones we don't hold are not immediately deleted.
        self.uniform_buffer = dingbat.create_buffer()
        self.uniform_block = dingbat.shader_uniform_blocks(shader)["SomeBlock"]
        self.draw_bindings = [
            dingbat.bind_attr_buffer(position_buffer, attrs["VertexPosition"], 3),
            dingbat.bind_uniform_buffer(self.uniform_buffer, self.uniform_block),
            dingbat.bind_draw_arrays(0, len(vertices)),
        ]

        dingbat.fill_uniform_block(self.uniform_buffer, self.uniform_block, dingbat.mat4())




def matrix_math_api_examples():
    """
    This is basically just an integration test.
    """
    
    print(dingbat.vec2())
    print(dingbat.vec2(array.array('f', (20, 40))))
    print(dingbat.mat3())
    print(dingbat.perspective_matrix(45, 640/480, 0.001, 100.0))
    print(dingbat.orthographic_matrix(-1.0, 1.0, -1.0, 1.0, 0.001, 100.0))
    print(dingbat.lookat_matrix(-20, -20, 20, 0, 0, 0, 0, 0, 1))
    translate = dingbat.translation_matrix(-10, -20, -30)
    rotate = dingbat.rotation_matrix(45, 0, 1, 0)
    scale = dingbat.scale_matrix(100, 200, 300)
    print(translate)
    print(rotate)
    print(scale)
    print(dingbat.multiply_matrices(translate, rotate, scale))



    
if __name__ == "__main__":
    if sys.version_info[0] < 3:
        print("Requires Python 3")
        exit(1)

    try:
        matrix_math_api_examples();
        dingbat.setup()
        with open("test.vert", "r") as vert_file:
            with open("test.frag", "r") as frag_file:
                shader = dingbat.build_shader(vert_file.read(), frag_file.read())
        assert(shader)
        dingbat.activate_shader(shader)

        teapot = DemoDrawable(shader, read_obj('teapot.obj'), [])
        draw_bindings = teapot.draw_bindings

        projection_matrix = dingbat.perspective_matrix(
            45, 640/480, 0.001, 100.0)

        view_matrix = dingbat.lookat_matrix(
            0, -4, 0, # camera position
            0, 0, 0,  # focal point
            0, 0, 1)  # up vector
        
        last = time.monotonic()
        while True:
            now = time.monotonic()
            delta = now - last
            last = now
            # print ("delta: %s" % str(1/delta))

            offset_a = dingbat.rotation_matrix(now * -1.5, 0, 0, 1)
            dingbat.fill_uniform_block(
                teapot.uniform_buffer,
                teapot.uniform_block,
                projection_matrix,
                view_matrix,
                offset_a)

            dingbat.clear()
            dingbat.batch_draw(*draw_bindings)
            dingbat.swap_buffers()
        
    except KeyboardInterrupt:
        dingbat.teardown()
        exit(0)
