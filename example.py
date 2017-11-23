

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

        dingbat.fill_uniform_block(self.uniform_buffer, self.uniform_block, 0.0)


        

if __name__ == "__main__":
    if sys.version_info[0] < 3:
        print("Requires Python 3")
        exit(1)

    print(dingbat.vec2())
    print(dingbat.vec2(array.array('f', (20, 40))))
    print(dingbat.mat3())
    try:
        dingbat.setup()
        with open("test.vert", "r") as vert_file:
            with open("test.frag", "r") as frag_file:
                shader = dingbat.build_shader(vert_file.read(), frag_file.read())
        assert(shader)
        dingbat.activate_shader(shader)


        clip_space_triangle = [
            0.0, 0.5, 0.0,
            -0.5, -0.5, 0.0,
            0.5, -0.5, 0.0
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
        
        last = time.monotonic()
        while True:
            now = time.monotonic()
            delta = now - last
            last = now
            #print ("delta: %s" % str(1/delta))

            offset_a = math.sin(now * 1.5) * 0.6
            offset_b = offset_a * -1
            dingbat.fill_uniform_block(entity_a.uniform_buffer, entity_a.uniform_block, offset_a)
            dingbat.fill_uniform_block(entity_b.uniform_buffer, entity_b.uniform_block, offset_b)

            dingbat.clear()
            dingbat.batch_draw(*draw_bindings)
            dingbat.swap_buffers()
        
    except KeyboardInterrupt:
        dingbat.teardown()
        exit(0)
