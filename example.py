

import sys
import time
import math
import array
import dingbat




class DemoDrawable:
    def __init__(self, shader, vertices, colors):
        position_buffer = dingbat.create_buffer()
        color_buffer = dingbat.create_buffer()
        attrs = dingbat.shader_attrs(shader)

        # https://docs.python.org/3.7/library/array.html
        dingbat.fill_buffer(position_buffer, array.array('f', vertices))
        dingbat.fill_buffer(color_buffer, array.array('f', colors))

        # We only need to hold on to the handle for the buffer objects
        # we intend to update.  The bindings below will ensure that
        # the ones we don't hold are not immediately deleted.
        self.uniform_buffer = dingbat.create_buffer()
        self.uniform_block = dingbat.shader_uniform_blocks(shader)["SomeBlock"]
        self.draw_bindings = [
            dingbat.bind_attr_buffer(position_buffer, attrs["VertexPosition"], 3),
            dingbat.bind_attr_buffer(color_buffer, attrs["VertexColor"], 3),
            dingbat.bind_uniform_buffer(self.uniform_buffer, self.uniform_block),
            dingbat.bind_draw_arrays(0, 9),
        ]

        dingbat.fill_uniform_block(self.uniform_buffer, self.uniform_block, dingbat.mat4())


        

if __name__ == "__main__":
    if sys.version_info[0] < 3:
        print("Requires Python 3")
        exit(1)

    # print(dingbat.vec2())
    # print(dingbat.vec2(array.array('f', (20, 40))))
    # print(dingbat.mat3())
    # print(dingbat.perspective_matrix(45, 640/480, 0.001, 100.0))
    # print(dingbat.orthographic_matrix(-1.0, 1.0, -1.0, 1.0, 0.001, 100.0))
    # print(dingbat.lookat_matrix(-20, -20, 20, 0, 0, 0, 0, 0, 1))
    # translate = dingbat.translation_matrix(-10, -20, -30)
    # rotate = dingbat.rotation_matrix(45, 0, 1, 0)
    # scale = dingbat.scale_matrix(100, 200, 300)
    # print(translate)
    # print(rotate)
    # print(scale)
    # print(dingbat.multiply_matrices(translate, rotate, scale))
    try:
        dingbat.setup()
        with open("test.vert", "r") as vert_file:
            with open("test.frag", "r") as frag_file:
                shader = dingbat.build_shader(vert_file.read(), frag_file.read())
        assert(shader)
        dingbat.activate_shader(shader)


        clip_space_triangle = [
            0.0, 0.0, 0.5,
            -0.5, 0.0, -0.5,
            0.5, 0.0, -0.5,
        ]

        vertex_colors_primary = [
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0
        ]

        vertex_colors_secondary = [
            1.0, 1.0, 0.0,
            0.0, 1.0, 1.0,
            1.0, 0.0, 1.0
        ]

        entity_a = DemoDrawable(shader, clip_space_triangle, vertex_colors_primary)
        entity_b = DemoDrawable(shader, clip_space_triangle, vertex_colors_secondary)
        draw_bindings = entity_a.draw_bindings + entity_b.draw_bindings

        camera_matrix = dingbat.multiply_matrices(
            dingbat.perspective_matrix(45, 640/480, 0.001, 100.0),
            dingbat.lookat_matrix(0, -1, 0, 0, 0, 0, 0, 0, 1))
        
        last = time.monotonic()
        while True:
            now = time.monotonic()
            delta = now - last
            last = now
            # print ("delta: %s" % str(1/delta))

            offset_a = dingbat.rotation_matrix(now * 1.5, 0, 1, 0)
            offset_b = dingbat.rotation_matrix(now * -1.5, 0, 1, 0)
            
            dingbat.fill_uniform_block(entity_a.uniform_buffer, entity_a.uniform_block, camera_matrix, offset_a)
            dingbat.fill_uniform_block(entity_b.uniform_buffer, entity_b.uniform_block, camera_matrix, offset_b)

            dingbat.clear()
            dingbat.batch_draw(*draw_bindings)
            dingbat.swap_buffers()
        
    except KeyboardInterrupt:
        dingbat.teardown()
        exit(0)
