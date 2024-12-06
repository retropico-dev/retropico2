//
// Created by cpasjuste on 06/12/24.
//

const char *libretro_xrgb_shader = R"text(
#if defined(VERTEX)

attribute vec2 TexCoord;
attribute vec2 VertexCoord;
attribute vec4 Color;
uniform mat4 MVPMatrix;
varying vec2 tex_coord;

void main() {
    gl_Position = MVPMatrix * vec4(VertexCoord, 0.0, 1.0);
    tex_coord = TexCoord;
}

#elif defined(FRAGMENT)

uniform sampler2D Texture;
varying mediump vec2 tex_coord;

void main() {
    gl_FragColor = vec4(texture2D(Texture, tex_coord).bgr, 1.0);
}

#endif
)text";
