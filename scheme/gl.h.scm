;;; generated from the distributed gl3.h by cgl on Mon Dec 17 00:01:33 CET 2012.
(define gl#es-version-3-0                                1)
(define gl#es-version-2-0                                1)
(define gl#depth-buffer-bit                              #x00000100)
(define gl#stencil-buffer-bit                            #x00000400)
(define gl#color-buffer-bit                              #x00004000)
(define gl#false                                         0)
(define gl#true                                          1)
(define gl#points                                        #x0000)
(define gl#lines                                         #x0001)
(define gl#line-loop                                     #x0002)
(define gl#line-strip                                    #x0003)
(define gl#triangles                                     #x0004)
(define gl#triangle-strip                                #x0005)
(define gl#triangle-fan                                  #x0006)
(define gl#zero                                          0)
(define gl#one                                           1)
(define gl#src-color                                     #x0300)
(define gl#one-minus-src-color                           #x0301)
(define gl#src-alpha                                     #x0302)
(define gl#one-minus-src-alpha                           #x0303)
(define gl#dst-alpha                                     #x0304)
(define gl#one-minus-dst-alpha                           #x0305)
(define gl#dst-color                                     #x0306)
(define gl#one-minus-dst-color                           #x0307)
(define gl#src-alpha-saturate                            #x0308)
(define gl#func-add                                      #x8006)
(define gl#blend-equation                                #x8009)
(define gl#blend-equation-alpha                          #x883D)
(define gl#func-subtract                                 #x800A)
(define gl#func-reverse-subtract                         #x800B)
(define gl#blend-dst-rgb                                 #x80C8)
(define gl#blend-src-rgb                                 #x80C9)
(define gl#blend-dst-alpha                               #x80CA)
(define gl#blend-src-alpha                               #x80CB)
(define gl#constant-color                                #x8001)
(define gl#one-minus-constant-color                      #x8002)
(define gl#constant-alpha                                #x8003)
(define gl#one-minus-constant-alpha                      #x8004)
(define gl#blend-color                                   #x8005)
(define gl#array-buffer                                  #x8892)
(define gl#element-array-buffer                          #x8893)
(define gl#array-buffer-binding                          #x8894)
(define gl#element-array-buffer-binding                  #x8895)
(define gl#stream-draw                                   #x88E0)
(define gl#static-draw                                   #x88E4)
(define gl#dynamic-draw                                  #x88E8)
(define gl#buffer-size                                   #x8764)
(define gl#buffer-usage                                  #x8765)
(define gl#current-vertex-attrib                         #x8626)
(define gl#front                                         #x0404)
(define gl#back                                          #x0405)
(define gl#front-and-back                                #x0408)
(define gl#texture-2d                                    #x0DE1)
(define gl#cull-face                                     #x0B44)
(define gl#blend                                         #x0BE2)
(define gl#dither                                        #x0BD0)
(define gl#stencil-test                                  #x0B90)
(define gl#depth-test                                    #x0B71)
(define gl#scissor-test                                  #x0C11)
(define gl#polygon-offset-fill                           #x8037)
(define gl#sample-alpha-to-coverage                      #x809E)
(define gl#sample-coverage                               #x80A0)
(define gl#no-error                                      0)
(define gl#invalid-enum                                  #x0500)
(define gl#invalid-value                                 #x0501)
(define gl#invalid-operation                             #x0502)
(define gl#out-of-memory                                 #x0505)
(define gl#cw                                            #x0900)
(define gl#ccw                                           #x0901)
(define gl#line-width                                    #x0B21)
(define gl#aliased-point-size-range                      #x846D)
(define gl#aliased-line-width-range                      #x846E)
(define gl#cull-face-mode                                #x0B45)
(define gl#front-face                                    #x0B46)
(define gl#depth-range                                   #x0B70)
(define gl#depth-writemask                               #x0B72)
(define gl#depth-clear-value                             #x0B73)
(define gl#depth-func                                    #x0B74)
(define gl#stencil-clear-value                           #x0B91)
(define gl#stencil-func                                  #x0B92)
(define gl#stencil-fail                                  #x0B94)
(define gl#stencil-pass-depth-fail                       #x0B95)
(define gl#stencil-pass-depth-pass                       #x0B96)
(define gl#stencil-ref                                   #x0B97)
(define gl#stencil-value-mask                            #x0B93)
(define gl#stencil-writemask                             #x0B98)
(define gl#stencil-back-func                             #x8800)
(define gl#stencil-back-fail                             #x8801)
(define gl#stencil-back-pass-depth-fail                  #x8802)
(define gl#stencil-back-pass-depth-pass                  #x8803)
(define gl#stencil-back-ref                              #x8CA3)
(define gl#stencil-back-value-mask                       #x8CA4)
(define gl#stencil-back-writemask                        #x8CA5)
(define gl#viewport                                      #x0BA2)
(define gl#scissor-box                                   #x0C10)
(define gl#color-clear-value                             #x0C22)
(define gl#color-writemask                               #x0C23)
(define gl#unpack-alignment                              #x0CF5)
(define gl#pack-alignment                                #x0D05)
(define gl#max-texture-size                              #x0D33)
(define gl#max-viewport-dims                             #x0D3A)
(define gl#subpixel-bits                                 #x0D50)
(define gl#red-bits                                      #x0D52)
(define gl#green-bits                                    #x0D53)
(define gl#blue-bits                                     #x0D54)
(define gl#alpha-bits                                    #x0D55)
(define gl#depth-bits                                    #x0D56)
(define gl#stencil-bits                                  #x0D57)
(define gl#polygon-offset-units                          #x2A00)
(define gl#polygon-offset-factor                         #x8038)
(define gl#texture-binding-2d                            #x8069)
(define gl#sample-buffers                                #x80A8)
(define gl#samples                                       #x80A9)
(define gl#sample-coverage-value                         #x80AA)
(define gl#sample-coverage-invert                        #x80AB)
(define gl#num-compressed-texture-formats                #x86A2)
(define gl#compressed-texture-formats                    #x86A3)
(define gl#dont-care                                     #x1100)
(define gl#fastest                                       #x1101)
(define gl#nicest                                        #x1102)
(define gl#generate-mipmap-hint                          #x8192)
(define gl#byte                                          #x1400)
(define gl#unsigned-byte                                 #x1401)
(define gl#short                                         #x1402)
(define gl#unsigned-short                                #x1403)
(define gl#int                                           #x1404)
(define gl#unsigned-int                                  #x1405)
(define gl#float                                         #x1406)
(define gl#fixed                                         #x140C)
(define gl#depth-component                               #x1902)
(define gl#alpha                                         #x1906)
(define gl#rgb                                           #x1907)
(define gl#rgba                                          #x1908)
(define gl#luminance                                     #x1909)
(define gl#luminance-alpha                               #x190A)
(define gl#unsigned-short-4-4-4-4                        #x8033)
(define gl#unsigned-short-5-5-5-1                        #x8034)
(define gl#unsigned-short-5-6-5                          #x8363)
(define gl#fragment-shader                               #x8B30)
(define gl#vertex-shader                                 #x8B31)
(define gl#max-vertex-attribs                            #x8869)
(define gl#max-vertex-uniform-vectors                    #x8DFB)
(define gl#max-varying-vectors                           #x8DFC)
(define gl#max-combined-texture-image-units              #x8B4D)
(define gl#max-vertex-texture-image-units                #x8B4C)
(define gl#max-texture-image-units                       #x8872)
(define gl#max-fragment-uniform-vectors                  #x8DFD)
(define gl#shader-type                                   #x8B4F)
(define gl#delete-status                                 #x8B80)
(define gl#link-status                                   #x8B82)
(define gl#validate-status                               #x8B83)
(define gl#attached-shaders                              #x8B85)
(define gl#active-uniforms                               #x8B86)
(define gl#active-uniform-max-length                     #x8B87)
(define gl#active-attributes                             #x8B89)
(define gl#active-attribute-max-length                   #x8B8A)
(define gl#shading-language-version                      #x8B8C)
(define gl#current-program                               #x8B8D)
(define gl#never                                         #x0200)
(define gl#less                                          #x0201)
(define gl#equal                                         #x0202)
(define gl#lequal                                        #x0203)
(define gl#greater                                       #x0204)
(define gl#notequal                                      #x0205)
(define gl#gequal                                        #x0206)
(define gl#always                                        #x0207)
(define gl#keep                                          #x1E00)
(define gl#replace                                       #x1E01)
(define gl#incr                                          #x1E02)
(define gl#decr                                          #x1E03)
(define gl#invert                                        #x150A)
(define gl#incr-wrap                                     #x8507)
(define gl#decr-wrap                                     #x8508)
(define gl#vendor                                        #x1F00)
(define gl#renderer                                      #x1F01)
(define gl#version                                       #x1F02)
(define gl#extensions                                    #x1F03)
(define gl#nearest                                       #x2600)
(define gl#linear                                        #x2601)
(define gl#nearest-mipmap-nearest                        #x2700)
(define gl#linear-mipmap-nearest                         #x2701)
(define gl#nearest-mipmap-linear                         #x2702)
(define gl#linear-mipmap-linear                          #x2703)
(define gl#texture-mag-filter                            #x2800)
(define gl#texture-min-filter                            #x2801)
(define gl#texture-wrap-s                                #x2802)
(define gl#texture-wrap-t                                #x2803)
(define gl#texture                                       #x1702)
(define gl#texture-cube-map                              #x8513)
(define gl#texture-binding-cube-map                      #x8514)
(define gl#texture-cube-map-positive-x                   #x8515)
(define gl#texture-cube-map-negative-x                   #x8516)
(define gl#texture-cube-map-positive-y                   #x8517)
(define gl#texture-cube-map-negative-y                   #x8518)
(define gl#texture-cube-map-positive-z                   #x8519)
(define gl#texture-cube-map-negative-z                   #x851A)
(define gl#max-cube-map-texture-size                     #x851C)
(define gl#texture0                                      #x84C0)
(define gl#texture1                                      #x84C1)
(define gl#texture2                                      #x84C2)
(define gl#texture3                                      #x84C3)
(define gl#texture4                                      #x84C4)
(define gl#texture5                                      #x84C5)
(define gl#texture6                                      #x84C6)
(define gl#texture7                                      #x84C7)
(define gl#texture8                                      #x84C8)
(define gl#texture9                                      #x84C9)
(define gl#texture10                                     #x84CA)
(define gl#texture11                                     #x84CB)
(define gl#texture12                                     #x84CC)
(define gl#texture13                                     #x84CD)
(define gl#texture14                                     #x84CE)
(define gl#texture15                                     #x84CF)
(define gl#texture16                                     #x84D0)
(define gl#texture17                                     #x84D1)
(define gl#texture18                                     #x84D2)
(define gl#texture19                                     #x84D3)
(define gl#texture20                                     #x84D4)
(define gl#texture21                                     #x84D5)
(define gl#texture22                                     #x84D6)
(define gl#texture23                                     #x84D7)
(define gl#texture24                                     #x84D8)
(define gl#texture25                                     #x84D9)
(define gl#texture26                                     #x84DA)
(define gl#texture27                                     #x84DB)
(define gl#texture28                                     #x84DC)
(define gl#texture29                                     #x84DD)
(define gl#texture30                                     #x84DE)
(define gl#texture31                                     #x84DF)
(define gl#active-texture                                #x84E0)
(define gl#repeat                                        #x2901)
(define gl#clamp-to-edge                                 #x812F)
(define gl#mirrored-repeat                               #x8370)
(define gl#float-vec2                                    #x8B50)
(define gl#float-vec3                                    #x8B51)
(define gl#float-vec4                                    #x8B52)
(define gl#int-vec2                                      #x8B53)
(define gl#int-vec3                                      #x8B54)
(define gl#int-vec4                                      #x8B55)
(define gl#bool                                          #x8B56)
(define gl#bool-vec2                                     #x8B57)
(define gl#bool-vec3                                     #x8B58)
(define gl#bool-vec4                                     #x8B59)
(define gl#float-mat2                                    #x8B5A)
(define gl#float-mat3                                    #x8B5B)
(define gl#float-mat4                                    #x8B5C)
(define gl#sampler-2d                                    #x8B5E)
(define gl#sampler-cube                                  #x8B60)
(define gl#vertex-attrib-array-enabled                   #x8622)
(define gl#vertex-attrib-array-size                      #x8623)
(define gl#vertex-attrib-array-stride                    #x8624)
(define gl#vertex-attrib-array-type                      #x8625)
(define gl#vertex-attrib-array-normalized                #x886A)
(define gl#vertex-attrib-array-pointer                   #x8645)
(define gl#vertex-attrib-array-buffer-binding            #x889F)
(define gl#implementation-color-read-type                #x8B9A)
(define gl#implementation-color-read-format              #x8B9B)
(define gl#compile-status                                #x8B81)
(define gl#info-log-length                               #x8B84)
(define gl#shader-source-length                          #x8B88)
(define gl#shader-compiler                               #x8DFA)
(define gl#shader-binary-formats                         #x8DF8)
(define gl#num-shader-binary-formats                     #x8DF9)
(define gl#low-float                                     #x8DF0)
(define gl#medium-float                                  #x8DF1)
(define gl#high-float                                    #x8DF2)
(define gl#low-int                                       #x8DF3)
(define gl#medium-int                                    #x8DF4)
(define gl#high-int                                      #x8DF5)
(define gl#framebuffer                                   #x8D40)
(define gl#renderbuffer                                  #x8D41)
(define gl#rgba4                                         #x8056)
(define gl#rgb5-a1                                       #x8057)
(define gl#rgb565                                        #x8D62)
(define gl#depth-component16                             #x81A5)
(define gl#stencil-index8                                #x8D48)
(define gl#renderbuffer-width                            #x8D42)
(define gl#renderbuffer-height                           #x8D43)
(define gl#renderbuffer-internal-format                  #x8D44)
(define gl#renderbuffer-red-size                         #x8D50)
(define gl#renderbuffer-green-size                       #x8D51)
(define gl#renderbuffer-blue-size                        #x8D52)
(define gl#renderbuffer-alpha-size                       #x8D53)
(define gl#renderbuffer-depth-size                       #x8D54)
(define gl#renderbuffer-stencil-size                     #x8D55)
(define gl#framebuffer-attachment-object-type            #x8CD0)
(define gl#framebuffer-attachment-object-name            #x8CD1)
(define gl#framebuffer-attachment-texture-level          #x8CD2)
(define gl#framebuffer-attachment-texture-cube-map-face  #x8CD3)
(define gl#color-attachment0                             #x8CE0)
(define gl#depth-attachment                              #x8D00)
(define gl#stencil-attachment                            #x8D20)
(define gl#none                                          0)
(define gl#framebuffer-complete                          #x8CD5)
(define gl#framebuffer-incomplete-attachment             #x8CD6)
(define gl#framebuffer-incomplete-missing-attachment     #x8CD7)
(define gl#framebuffer-incomplete-dimensions             #x8CD9)
(define gl#framebuffer-unsupported                       #x8CDD)
(define gl#framebuffer-binding                           #x8CA6)
(define gl#renderbuffer-binding                          #x8CA7)
(define gl#max-renderbuffer-size                         #x84E8)
(define gl#invalid-framebuffer-operation                 #x0506)
(define gl#read-buffer                                   #x0C02)
(define gl#unpack-row-length                             #x0CF2)
(define gl#unpack-skip-rows                              #x0CF3)
(define gl#unpack-skip-pixels                            #x0CF4)
(define gl#pack-row-length                               #x0D02)
(define gl#pack-skip-rows                                #x0D03)
(define gl#pack-skip-pixels                              #x0D04)
(define gl#color                                         #x1800)
(define gl#depth                                         #x1801)
(define gl#stencil                                       #x1802)
(define gl#red                                           #x1903)
(define gl#rgb8                                          #x8051)
(define gl#rgba8                                         #x8058)
(define gl#rgb10-a2                                      #x8059)
(define gl#texture-binding-3d                            #x806A)
(define gl#pack-skip-images                              #x806B)
(define gl#pack-image-height                             #x806C)
(define gl#unpack-skip-images                            #x806D)
(define gl#unpack-image-height                           #x806E)
(define gl#texture-3d                                    #x806F)
(define gl#texture-wrap-r                                #x8072)
(define gl#max-3d-texture-size                           #x8073)
(define gl#unsigned-int-2-10-10-10-rev                   #x8368)
(define gl#max-elements-vertices                         #x80E8)
(define gl#max-elements-indices                          #x80E9)
(define gl#texture-min-lod                               #x813A)
(define gl#texture-max-lod                               #x813B)
(define gl#texture-base-level                            #x813C)
(define gl#texture-max-level                             #x813D)
(define gl#min                                           #x8007)
(define gl#max                                           #x8008)
(define gl#depth-component24                             #x81A6)
(define gl#max-texture-lod-bias                          #x84FD)
(define gl#texture-compare-mode                          #x884C)
(define gl#texture-compare-func                          #x884D)
(define gl#current-query                                 #x8865)
(define gl#query-result                                  #x8866)
(define gl#query-result-available                        #x8867)
(define gl#buffer-mapped                                 #x88BC)
(define gl#buffer-map-pointer                            #x88BD)
(define gl#stream-read                                   #x88E1)
(define gl#stream-copy                                   #x88E2)
(define gl#static-read                                   #x88E5)
(define gl#static-copy                                   #x88E6)
(define gl#dynamic-read                                  #x88E9)
(define gl#dynamic-copy                                  #x88EA)
(define gl#max-draw-buffers                              #x8824)
(define gl#draw-buffer0                                  #x8825)
(define gl#draw-buffer1                                  #x8826)
(define gl#draw-buffer2                                  #x8827)
(define gl#draw-buffer3                                  #x8828)
(define gl#draw-buffer4                                  #x8829)
(define gl#draw-buffer5                                  #x882A)
(define gl#draw-buffer6                                  #x882B)
(define gl#draw-buffer7                                  #x882C)
(define gl#draw-buffer8                                  #x882D)
(define gl#draw-buffer9                                  #x882E)
(define gl#draw-buffer10                                 #x882F)
(define gl#draw-buffer11                                 #x8830)
(define gl#draw-buffer12                                 #x8831)
(define gl#draw-buffer13                                 #x8832)
(define gl#draw-buffer14                                 #x8833)
(define gl#draw-buffer15                                 #x8834)
(define gl#max-fragment-uniform-components               #x8B49)
(define gl#max-vertex-uniform-components                 #x8B4A)
(define gl#sampler-3d                                    #x8B5F)
(define gl#sampler-2d-shadow                             #x8B62)
(define gl#fragment-shader-derivative-hint               #x8B8B)
(define gl#pixel-pack-buffer                             #x88EB)
(define gl#pixel-unpack-buffer                           #x88EC)
(define gl#pixel-pack-buffer-binding                     #x88ED)
(define gl#pixel-unpack-buffer-binding                   #x88EF)
(define gl#float-mat2x3                                  #x8B65)
(define gl#float-mat2x4                                  #x8B66)
(define gl#float-mat3x2                                  #x8B67)
(define gl#float-mat3x4                                  #x8B68)
(define gl#float-mat4x2                                  #x8B69)
(define gl#float-mat4x3                                  #x8B6A)
(define gl#srgb                                          #x8C40)
(define gl#srgb8                                         #x8C41)
(define gl#srgb8-alpha8                                  #x8C43)
(define gl#compare-ref-to-texture                        #x884E)
(define gl#major-version                                 #x821B)
(define gl#minor-version                                 #x821C)
(define gl#num-extensions                                #x821D)
(define gl#rgba32f                                       #x8814)
(define gl#rgb32f                                        #x8815)
(define gl#rgba16f                                       #x881A)
(define gl#rgb16f                                        #x881B)
(define gl#vertex-attrib-array-integer                   #x88FD)
(define gl#max-array-texture-layers                      #x88FF)
(define gl#min-program-texel-offset                      #x8904)
(define gl#max-program-texel-offset                      #x8905)
(define gl#max-varying-components                        #x8B4B)
(define gl#texture-2d-array                              #x8C1A)
(define gl#texture-binding-2d-array                      #x8C1D)
(define gl#r11f-g11f-b10f                                #x8C3A)
(define gl#unsigned-int-10f-11f-11f-rev                  #x8C3B)
(define gl#rgb9-e5                                       #x8C3D)
(define gl#unsigned-int-5-9-9-9-rev                      #x8C3E)
(define gl#transform-feedback-varying-max-length         #x8C76)
(define gl#transform-feedback-buffer-mode                #x8C7F)
(define gl#max-transform-feedback-separate-components    #x8C80)
(define gl#transform-feedback-varyings                   #x8C83)
(define gl#transform-feedback-buffer-start               #x8C84)
(define gl#transform-feedback-buffer-size                #x8C85)
(define gl#transform-feedback-primitives-written         #x8C88)
(define gl#rasterizer-discard                            #x8C89)
(define gl#max-transform-feedback-interleaved-components #x8C8A)
(define gl#max-transform-feedback-separate-attribs       #x8C8B)
(define gl#interleaved-attribs                           #x8C8C)
(define gl#separate-attribs                              #x8C8D)
(define gl#transform-feedback-buffer                     #x8C8E)
(define gl#transform-feedback-buffer-binding             #x8C8F)
(define gl#rgba32ui                                      #x8D70)
(define gl#rgb32ui                                       #x8D71)
(define gl#rgba16ui                                      #x8D76)
(define gl#rgb16ui                                       #x8D77)
(define gl#rgba8ui                                       #x8D7C)
(define gl#rgb8ui                                        #x8D7D)
(define gl#rgba32i                                       #x8D82)
(define gl#rgb32i                                        #x8D83)
(define gl#rgba16i                                       #x8D88)
(define gl#rgb16i                                        #x8D89)
(define gl#rgba8i                                        #x8D8E)
(define gl#rgb8i                                         #x8D8F)
(define gl#red-integer                                   #x8D94)
(define gl#rgb-integer                                   #x8D98)
(define gl#rgba-integer                                  #x8D99)
(define gl#sampler-2d-array                              #x8DC1)
(define gl#sampler-2d-array-shadow                       #x8DC4)
(define gl#sampler-cube-shadow                           #x8DC5)
(define gl#unsigned-int-vec2                             #x8DC6)
(define gl#unsigned-int-vec3                             #x8DC7)
(define gl#unsigned-int-vec4                             #x8DC8)
(define gl#int-sampler-2d                                #x8DCA)
(define gl#int-sampler-3d                                #x8DCB)
(define gl#int-sampler-cube                              #x8DCC)
(define gl#int-sampler-2d-array                          #x8DCF)
(define gl#unsigned-int-sampler-2d                       #x8DD2)
(define gl#unsigned-int-sampler-3d                       #x8DD3)
(define gl#unsigned-int-sampler-cube                     #x8DD4)
(define gl#unsigned-int-sampler-2d-array                 #x8DD7)
(define gl#buffer-access-flags                           #x911F)
(define gl#buffer-map-length                             #x9120)
(define gl#buffer-map-offset                             #x9121)
(define gl#depth-component32f                            #x8CAC)
(define gl#depth32f-stencil8                             #x8CAD)
(define gl#float-32-unsigned-int-24-8-rev                #x8DAD)
(define gl#framebuffer-attachment-color-encoding         #x8210)
(define gl#framebuffer-attachment-component-type         #x8211)
(define gl#framebuffer-attachment-red-size               #x8212)
(define gl#framebuffer-attachment-green-size             #x8213)
(define gl#framebuffer-attachment-blue-size              #x8214)
(define gl#framebuffer-attachment-alpha-size             #x8215)
(define gl#framebuffer-attachment-depth-size             #x8216)
(define gl#framebuffer-attachment-stencil-size           #x8217)
(define gl#framebuffer-default                           #x8218)
(define gl#framebuffer-undefined                         #x8219)
(define gl#depth-stencil-attachment                      #x821A)
(define gl#depth-stencil                                 #x84F9)
(define gl#unsigned-int-24-8                             #x84FA)
(define gl#depth24-stencil8                              #x88F0)
(define gl#unsigned-normalized                           #x8C17)
(define gl#draw-framebuffer-binding                      gl#framebuffer-binding)
(define gl#read-framebuffer                              #x8CA8)
(define gl#draw-framebuffer                              #x8CA9)
(define gl#read-framebuffer-binding                      #x8CAA)
(define gl#renderbuffer-samples                          #x8CAB)
(define gl#framebuffer-attachment-texture-layer          #x8CD4)
(define gl#max-color-attachments                         #x8CDF)
(define gl#color-attachment1                             #x8CE1)
(define gl#color-attachment2                             #x8CE2)
(define gl#color-attachment3                             #x8CE3)
(define gl#color-attachment4                             #x8CE4)
(define gl#color-attachment5                             #x8CE5)
(define gl#color-attachment6                             #x8CE6)
(define gl#color-attachment7                             #x8CE7)
(define gl#color-attachment8                             #x8CE8)
(define gl#color-attachment9                             #x8CE9)
(define gl#color-attachment10                            #x8CEA)
(define gl#color-attachment11                            #x8CEB)
(define gl#color-attachment12                            #x8CEC)
(define gl#color-attachment13                            #x8CED)
(define gl#color-attachment14                            #x8CEE)
(define gl#color-attachment15                            #x8CEF)
(define gl#framebuffer-incomplete-multisample            #x8D56)
(define gl#max-samples                                   #x8D57)
(define gl#half-float                                    #x140B)
(define gl#map-read-bit                                  #x0001)
(define gl#map-write-bit                                 #x0002)
(define gl#map-invalidate-range-bit                      #x0004)
(define gl#map-invalidate-buffer-bit                     #x0008)
(define gl#map-flush-explicit-bit                        #x0010)
(define gl#map-unsynchronized-bit                        #x0020)
(define gl#rg                                            #x8227)
(define gl#rg-integer                                    #x8228)
(define gl#r8                                            #x8229)
(define gl#rg8                                           #x822B)
(define gl#r16f                                          #x822D)
(define gl#r32f                                          #x822E)
(define gl#rg16f                                         #x822F)
(define gl#rg32f                                         #x8230)
(define gl#r8i                                           #x8231)
(define gl#r8ui                                          #x8232)
(define gl#r16i                                          #x8233)
(define gl#r16ui                                         #x8234)
(define gl#r32i                                          #x8235)
(define gl#r32ui                                         #x8236)
(define gl#rg8i                                          #x8237)
(define gl#rg8ui                                         #x8238)
(define gl#rg16i                                         #x8239)
(define gl#rg16ui                                        #x823A)
(define gl#rg32i                                         #x823B)
(define gl#rg32ui                                        #x823C)
(define gl#vertex-array-binding                          #x85B5)
(define gl#r8-snorm                                      #x8F94)
(define gl#rg8-snorm                                     #x8F95)
(define gl#rgb8-snorm                                    #x8F96)
(define gl#rgba8-snorm                                   #x8F97)
(define gl#signed-normalized                             #x8F9C)
(define gl#primitive-restart-fixed-index                 #x8D69)
(define gl#copy-read-buffer                              #x8F36)
(define gl#copy-write-buffer                             #x8F37)
(define gl#copy-read-buffer-binding                      gl#copy-read-buffer)
(define gl#copy-write-buffer-binding                     gl#copy-write-buffer)
(define gl#uniform-buffer                                #x8A11)
(define gl#uniform-buffer-binding                        #x8A28)
(define gl#uniform-buffer-start                          #x8A29)
(define gl#uniform-buffer-size                           #x8A2A)
(define gl#max-vertex-uniform-blocks                     #x8A2B)
(define gl#max-fragment-uniform-blocks                   #x8A2D)
(define gl#max-combined-uniform-blocks                   #x8A2E)
(define gl#max-uniform-buffer-bindings                   #x8A2F)
(define gl#max-uniform-block-size                        #x8A30)
(define gl#max-combined-vertex-uniform-components        #x8A31)
(define gl#max-combined-fragment-uniform-components      #x8A33)
(define gl#uniform-buffer-offset-alignment               #x8A34)
(define gl#active-uniform-block-max-name-length          #x8A35)
(define gl#active-uniform-blocks                         #x8A36)
(define gl#uniform-type                                  #x8A37)
(define gl#uniform-size                                  #x8A38)
(define gl#uniform-name-length                           #x8A39)
(define gl#uniform-block-index                           #x8A3A)
(define gl#uniform-offset                                #x8A3B)
(define gl#uniform-array-stride                          #x8A3C)
(define gl#uniform-matrix-stride                         #x8A3D)
(define gl#uniform-is-row-major                          #x8A3E)
(define gl#uniform-block-binding                         #x8A3F)
(define gl#uniform-block-data-size                       #x8A40)
(define gl#uniform-block-name-length                     #x8A41)
(define gl#uniform-block-active-uniforms                 #x8A42)
(define gl#uniform-block-active-uniform-indices          #x8A43)
(define gl#uniform-block-referenced-by-vertex-shader     #x8A44)
(define gl#uniform-block-referenced-by-fragment-shader   #x8A46)
(define gl#invalid-index                                 #xffffffff)
(define gl#max-vertex-output-components                  #x9122)
(define gl#max-fragment-input-components                 #x9125)
(define gl#max-server-wait-timeout                       #x9111)
(define gl#object-type                                   #x9112)
(define gl#sync-condition                                #x9113)
(define gl#sync-status                                   #x9114)
(define gl#sync-flags                                    #x9115)
(define gl#sync-fence                                    #x9116)
(define gl#sync-gpu-commands-complete                    #x9117)
(define gl#unsignaled                                    #x9118)
(define gl#signaled                                      #x9119)
(define gl#already-signaled                              #x911A)
(define gl#timeout-expired                               #x911B)
(define gl#condition-satisfied                           #x911C)
(define gl#wait-failed                                   #x911D)
(define gl#sync-flush-commands-bit                       #x00000001)
(define gl#timeout-ignored                               #xffffffffffffffff)
(define gl#vertex-attrib-array-divisor                   #x88FE)
(define gl#any-samples-passed                            #x8C2F)
(define gl#any-samples-passed-conservative               #x8D6A)
(define gl#sampler-binding                               #x8919)
(define gl#rgb10-a2ui                                    #x906F)
(define gl#texture-swizzle-r                             #x8E42)
(define gl#texture-swizzle-g                             #x8E43)
(define gl#texture-swizzle-b                             #x8E44)
(define gl#texture-swizzle-a                             #x8E45)
(define gl#green                                         #x1904)
(define gl#blue                                          #x1905)
(define gl#int-2-10-10-10-rev                            #x8D9F)
(define gl#transform-feedback                            #x8E22)
(define gl#transform-feedback-paused                     #x8E23)
(define gl#transform-feedback-active                     #x8E24)
(define gl#transform-feedback-binding                    #x8E25)
(define gl#program-binary-retrievable-hint               #x8257)
(define gl#program-binary-length                         #x8741)
(define gl#num-program-binary-formats                    #x87FE)
(define gl#program-binary-formats                        #x87FF)
(define gl#compressed-r11-eac                            #x9270)
(define gl#compressed-signed-r11-eac                     #x9271)
(define gl#compressed-rg11-eac                           #x9272)
(define gl#compressed-signed-rg11-eac                    #x9273)
(define gl#compressed-rgb8-etc2                          #x9274)
(define gl#compressed-srgb8-etc2                         #x9275)
(define gl#compressed-rgb8-punchthrough-alpha1-etc2      #x9276)
(define gl#compressed-srgb8-punchthrough-alpha1-etc2     #x9277)
(define gl#compressed-rgba8-etc2-eac                     #x9278)
(define gl#compressed-srgb8-alpha8-etc2-eac              #x9279)
(define gl#texture-immutable-format                      #x912F)
(define gl#max-element-index                             #x8D6B)
(define gl#num-sample-counts                             #x9380)
(define gl#texture-immutable-levels                      #x8D63)
