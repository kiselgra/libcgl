;;; generated from the local gl.h by cgl on Mon Aug 20 12:48:28 CEST 2012.
(define gl:version-1-1 1)
(define gl:version-1-2 1)
(define gl:version-1-3 1)
(define gl:arb-imaging 1)
(define gl:false #x0)
(define gl:true #x1)
(define gl:byte #x1400)
(define gl:unsigned-byte #x1401)
(define gl:short #x1402)
(define gl:unsigned-short #x1403)
(define gl:int #x1404)
(define gl:unsigned-int #x1405)
(define gl:float #x1406)
(define gl:2-bytes #x1407)
(define gl:3-bytes #x1408)
(define gl:4-bytes #x1409)
(define gl:double #x140a)
(define gl:points #x0000)
(define gl:lines #x0001)
(define gl:line-loop #x0002)
(define gl:line-strip #x0003)
(define gl:triangles #x0004)
(define gl:triangle-strip #x0005)
(define gl:triangle-fan #x0006)
(define gl:quads #x0007)
(define gl:quad-strip #x0008)
(define gl:polygon #x0009)
(define gl:vertex-array #x8074)
(define gl:normal-array #x8075)
(define gl:color-array #x8076)
(define gl:index-array #x8077)
(define gl:texture-coord-array #x8078)
(define gl:edge-flag-array #x8079)
(define gl:vertex-array-size #x807a)
(define gl:vertex-array-type #x807b)
(define gl:vertex-array-stride #x807c)
(define gl:normal-array-type #x807e)
(define gl:normal-array-stride #x807f)
(define gl:color-array-size #x8081)
(define gl:color-array-type #x8082)
(define gl:color-array-stride #x8083)
(define gl:index-array-type #x8085)
(define gl:index-array-stride #x8086)
(define gl:texture-coord-array-size #x8088)
(define gl:texture-coord-array-type #x8089)
(define gl:texture-coord-array-stride #x808a)
(define gl:edge-flag-array-stride #x808c)
(define gl:vertex-array-pointer #x808e)
(define gl:normal-array-pointer #x808f)
(define gl:color-array-pointer #x8090)
(define gl:index-array-pointer #x8091)
(define gl:texture-coord-array-pointer #x8092)
(define gl:edge-flag-array-pointer #x8093)
(define gl:v2f #x2a20)
(define gl:v3f #x2a21)
(define gl:c4ub-v2f #x2a22)
(define gl:c4ub-v3f #x2a23)
(define gl:c3f-v3f #x2a24)
(define gl:n3f-v3f #x2a25)
(define gl:c4f-n3f-v3f #x2a26)
(define gl:t2f-v3f #x2a27)
(define gl:t4f-v4f #x2a28)
(define gl:t2f-c4ub-v3f #x2a29)
(define gl:t2f-c3f-v3f #x2a2a)
(define gl:t2f-n3f-v3f #x2a2b)
(define gl:t2f-c4f-n3f-v3f #x2a2c)
(define gl:t4f-c4f-n3f-v4f #x2a2d)
(define gl:matrix-mode #x0ba0)
(define gl:modelview #x1700)
(define gl:projection #x1701)
(define gl:texture #x1702)
(define gl:point-smooth #x0b10)
(define gl:point-size #x0b11)
(define gl:point-size-granularity #x0b13)
(define gl:point-size-range #x0b12)
(define gl:line-smooth #x0b20)
(define gl:line-stipple #x0b24)
(define gl:line-stipple-pattern #x0b25)
(define gl:line-stipple-repeat #x0b26)
(define gl:line-width #x0b21)
(define gl:line-width-granularity #x0b23)
(define gl:line-width-range #x0b22)
(define gl:point #x1b00)
(define gl:line #x1b01)
(define gl:fill #x1b02)
(define gl:cw #x0900)
(define gl:ccw #x0901)
(define gl:front #x0404)
(define gl:back #x0405)
(define gl:polygon-mode #x0b40)
(define gl:polygon-smooth #x0b41)
(define gl:polygon-stipple #x0b42)
(define gl:edge-flag #x0b43)
(define gl:cull-face #x0b44)
(define gl:cull-face-mode #x0b45)
(define gl:front-face #x0b46)
(define gl:polygon-offset-factor #x8038)
(define gl:polygon-offset-units #x2a00)
(define gl:polygon-offset-point #x2a01)
(define gl:polygon-offset-line #x2a02)
(define gl:polygon-offset-fill #x8037)
(define gl:compile #x1300)
(define gl:compile-and-execute #x1301)
(define gl:list-base #x0b32)
(define gl:list-index #x0b33)
(define gl:list-mode #x0b30)
(define gl:never #x0200)
(define gl:less #x0201)
(define gl:equal #x0202)
(define gl:lequal #x0203)
(define gl:greater #x0204)
(define gl:notequal #x0205)
(define gl:gequal #x0206)
(define gl:always #x0207)
(define gl:depth-test #x0b71)
(define gl:depth-bits #x0d56)
(define gl:depth-clear-value #x0b73)
(define gl:depth-func #x0b74)
(define gl:depth-range #x0b70)
(define gl:depth-writemask #x0b72)
(define gl:depth-component #x1902)
(define gl:lighting #x0b50)
(define gl:light0 #x4000)
(define gl:light1 #x4001)
(define gl:light2 #x4002)
(define gl:light3 #x4003)
(define gl:light4 #x4004)
(define gl:light5 #x4005)
(define gl:light6 #x4006)
(define gl:light7 #x4007)
(define gl:spot-exponent #x1205)
(define gl:spot-cutoff #x1206)
(define gl:constant-attenuation #x1207)
(define gl:linear-attenuation #x1208)
(define gl:quadratic-attenuation #x1209)
(define gl:ambient #x1200)
(define gl:diffuse #x1201)
(define gl:specular #x1202)
(define gl:shininess #x1601)
(define gl:emission #x1600)
(define gl:position #x1203)
(define gl:spot-direction #x1204)
(define gl:ambient-and-diffuse #x1602)
(define gl:color-indexes #x1603)
(define gl:light-model-two-side #x0b52)
(define gl:light-model-local-viewer #x0b51)
(define gl:light-model-ambient #x0b53)
(define gl:front-and-back #x0408)
(define gl:shade-model #x0b54)
(define gl:flat #x1d00)
(define gl:smooth #x1d01)
(define gl:color-material #x0b57)
(define gl:color-material-face #x0b55)
(define gl:color-material-parameter #x0b56)
(define gl:normalize #x0ba1)
(define gl:clip-plane0 #x3000)
(define gl:clip-plane1 #x3001)
(define gl:clip-plane2 #x3002)
(define gl:clip-plane3 #x3003)
(define gl:clip-plane4 #x3004)
(define gl:clip-plane5 #x3005)
(define gl:accum-red-bits #x0d58)
(define gl:accum-green-bits #x0d59)
(define gl:accum-blue-bits #x0d5a)
(define gl:accum-alpha-bits #x0d5b)
(define gl:accum-clear-value #x0b80)
(define gl:accum #x0100)
(define gl:add #x0104)
(define gl:load #x0101)
(define gl:mult #x0103)
(define gl:return #x0102)
(define gl:alpha-test #x0bc0)
(define gl:alpha-test-ref #x0bc2)
(define gl:alpha-test-func #x0bc1)
(define gl:blend #x0be2)
(define gl:blend-src #x0be1)
(define gl:blend-dst #x0be0)
(define gl:zero #x0)
(define gl:one #x1)
(define gl:src-color #x0300)
(define gl:one-minus-src-color #x0301)
(define gl:src-alpha #x0302)
(define gl:one-minus-src-alpha #x0303)
(define gl:dst-alpha #x0304)
(define gl:one-minus-dst-alpha #x0305)
(define gl:dst-color #x0306)
(define gl:one-minus-dst-color #x0307)
(define gl:src-alpha-saturate #x0308)
(define gl:feedback #x1c01)
(define gl:render #x1c00)
(define gl:select #x1c02)
(define gl:2d #x0600)
(define gl:3d #x0601)
(define gl:3d-color #x0602)
(define gl:3d-color-texture #x0603)
(define gl:4d-color-texture #x0604)
(define gl:point-token #x0701)
(define gl:line-token #x0702)
(define gl:line-reset-token #x0707)
(define gl:polygon-token #x0703)
(define gl:bitmap-token #x0704)
(define gl:draw-pixel-token #x0705)
(define gl:copy-pixel-token #x0706)
(define gl:pass-through-token #x0700)
(define gl:feedback-buffer-pointer #x0df0)
(define gl:feedback-buffer-size #x0df1)
(define gl:feedback-buffer-type #x0df2)
(define gl:selection-buffer-pointer #x0df3)
(define gl:selection-buffer-size #x0df4)
(define gl:fog #x0b60)
(define gl:fog-mode #x0b65)
(define gl:fog-density #x0b62)
(define gl:fog-color #x0b66)
(define gl:fog-index #x0b61)
(define gl:fog-start #x0b63)
(define gl:fog-end #x0b64)
(define gl:linear #x2601)
(define gl:exp #x0800)
(define gl:exp2 #x0801)
(define gl:logic-op #x0bf1)
(define gl:index-logic-op #x0bf1)
(define gl:color-logic-op #x0bf2)
(define gl:logic-op-mode #x0bf0)
(define gl:clear #x1500)
(define gl:set #x150f)
(define gl:copy #x1503)
(define gl:copy-inverted #x150c)
(define gl:noop #x1505)
(define gl:invert #x150a)
(define gl:and #x1501)
(define gl:nand #x150e)
(define gl:or #x1507)
(define gl:nor #x1508)
(define gl:xor #x1506)
(define gl:equiv #x1509)
(define gl:and-reverse #x1502)
(define gl:and-inverted #x1504)
(define gl:or-reverse #x150b)
(define gl:or-inverted #x150d)
(define gl:stencil-bits #x0d57)
(define gl:stencil-test #x0b90)
(define gl:stencil-clear-value #x0b91)
(define gl:stencil-func #x0b92)
(define gl:stencil-value-mask #x0b93)
(define gl:stencil-fail #x0b94)
(define gl:stencil-pass-depth-fail #x0b95)
(define gl:stencil-pass-depth-pass #x0b96)
(define gl:stencil-ref #x0b97)
(define gl:stencil-writemask #x0b98)
(define gl:stencil-index #x1901)
(define gl:keep #x1e00)
(define gl:replace #x1e01)
(define gl:incr #x1e02)
(define gl:decr #x1e03)
(define gl:none #x0)
(define gl:left #x0406)
(define gl:right #x0407)
(define gl:front-left #x0400)
(define gl:front-right #x0401)
(define gl:back-left #x0402)
(define gl:back-right #x0403)
(define gl:aux0 #x0409)
(define gl:aux1 #x040a)
(define gl:aux2 #x040b)
(define gl:aux3 #x040c)
(define gl:color-index #x1900)
(define gl:red #x1903)
(define gl:green #x1904)
(define gl:blue #x1905)
(define gl:alpha #x1906)
(define gl:luminance #x1909)
(define gl:luminance-alpha #x190a)
(define gl:alpha-bits #x0d55)
(define gl:red-bits #x0d52)
(define gl:green-bits #x0d53)
(define gl:blue-bits #x0d54)
(define gl:index-bits #x0d51)
(define gl:subpixel-bits #x0d50)
(define gl:aux-buffers #x0c00)
(define gl:read-buffer #x0c02)
(define gl:draw-buffer #x0c01)
(define gl:doublebuffer #x0c32)
(define gl:stereo #x0c33)
(define gl:bitmap #x1a00)
(define gl:color #x1800)
(define gl:depth #x1801)
(define gl:stencil #x1802)
(define gl:dither #x0bd0)
(define gl:rgb #x1907)
(define gl:rgba #x1908)
(define gl:max-list-nesting #x0b31)
(define gl:max-eval-order #x0d30)
(define gl:max-lights #x0d31)
(define gl:max-clip-planes #x0d32)
(define gl:max-texture-size #x0d33)
(define gl:max-pixel-map-table #x0d34)
(define gl:max-attrib-stack-depth #x0d35)
(define gl:max-modelview-stack-depth #x0d36)
(define gl:max-name-stack-depth #x0d37)
(define gl:max-projection-stack-depth #x0d38)
(define gl:max-texture-stack-depth #x0d39)
(define gl:max-viewport-dims #x0d3a)
(define gl:max-client-attrib-stack-depth #x0d3b)
(define gl:attrib-stack-depth #x0bb0)
(define gl:client-attrib-stack-depth #x0bb1)
(define gl:color-clear-value #x0c22)
(define gl:color-writemask #x0c23)
(define gl:current-index #x0b01)
(define gl:current-color #x0b00)
(define gl:current-normal #x0b02)
(define gl:current-raster-color #x0b04)
(define gl:current-raster-distance #x0b09)
(define gl:current-raster-index #x0b05)
(define gl:current-raster-position #x0b07)
(define gl:current-raster-texture-coords #x0b06)
(define gl:current-raster-position-valid #x0b08)
(define gl:current-texture-coords #x0b03)
(define gl:index-clear-value #x0c20)
(define gl:index-mode #x0c30)
(define gl:index-writemask #x0c21)
(define gl:modelview-matrix #x0ba6)
(define gl:modelview-stack-depth #x0ba3)
(define gl:name-stack-depth #x0d70)
(define gl:projection-matrix #x0ba7)
(define gl:projection-stack-depth #x0ba4)
(define gl:render-mode #x0c40)
(define gl:rgba-mode #x0c31)
(define gl:texture-matrix #x0ba8)
(define gl:texture-stack-depth #x0ba5)
(define gl:viewport #x0ba2)
(define gl:auto-normal #x0d80)
(define gl:map1-color-4 #x0d90)
(define gl:map1-index #x0d91)
(define gl:map1-normal #x0d92)
(define gl:map1-texture-coord-1 #x0d93)
(define gl:map1-texture-coord-2 #x0d94)
(define gl:map1-texture-coord-3 #x0d95)
(define gl:map1-texture-coord-4 #x0d96)
(define gl:map1-vertex-3 #x0d97)
(define gl:map1-vertex-4 #x0d98)
(define gl:map2-color-4 #x0db0)
(define gl:map2-index #x0db1)
(define gl:map2-normal #x0db2)
(define gl:map2-texture-coord-1 #x0db3)
(define gl:map2-texture-coord-2 #x0db4)
(define gl:map2-texture-coord-3 #x0db5)
(define gl:map2-texture-coord-4 #x0db6)
(define gl:map2-vertex-3 #x0db7)
(define gl:map2-vertex-4 #x0db8)
(define gl:map1-grid-domain #x0dd0)
(define gl:map1-grid-segments #x0dd1)
(define gl:map2-grid-domain #x0dd2)
(define gl:map2-grid-segments #x0dd3)
(define gl:coeff #x0a00)
(define gl:order #x0a01)
(define gl:domain #x0a02)
(define gl:perspective-correction-hint #x0c50)
(define gl:point-smooth-hint #x0c51)
(define gl:line-smooth-hint #x0c52)
(define gl:polygon-smooth-hint #x0c53)
(define gl:fog-hint #x0c54)
(define gl:dont-care #x1100)
(define gl:fastest #x1101)
(define gl:nicest #x1102)
(define gl:scissor-box #x0c10)
(define gl:scissor-test #x0c11)
(define gl:map-color #x0d10)
(define gl:map-stencil #x0d11)
(define gl:index-shift #x0d12)
(define gl:index-offset #x0d13)
(define gl:red-scale #x0d14)
(define gl:red-bias #x0d15)
(define gl:green-scale #x0d18)
(define gl:green-bias #x0d19)
(define gl:blue-scale #x0d1a)
(define gl:blue-bias #x0d1b)
(define gl:alpha-scale #x0d1c)
(define gl:alpha-bias #x0d1d)
(define gl:depth-scale #x0d1e)
(define gl:depth-bias #x0d1f)
(define gl:pixel-map-s-to-s-size #x0cb1)
(define gl:pixel-map-i-to-i-size #x0cb0)
(define gl:pixel-map-i-to-r-size #x0cb2)
(define gl:pixel-map-i-to-g-size #x0cb3)
(define gl:pixel-map-i-to-b-size #x0cb4)
(define gl:pixel-map-i-to-a-size #x0cb5)
(define gl:pixel-map-r-to-r-size #x0cb6)
(define gl:pixel-map-g-to-g-size #x0cb7)
(define gl:pixel-map-b-to-b-size #x0cb8)
(define gl:pixel-map-a-to-a-size #x0cb9)
(define gl:pixel-map-s-to-s #x0c71)
(define gl:pixel-map-i-to-i #x0c70)
(define gl:pixel-map-i-to-r #x0c72)
(define gl:pixel-map-i-to-g #x0c73)
(define gl:pixel-map-i-to-b #x0c74)
(define gl:pixel-map-i-to-a #x0c75)
(define gl:pixel-map-r-to-r #x0c76)
(define gl:pixel-map-g-to-g #x0c77)
(define gl:pixel-map-b-to-b #x0c78)
(define gl:pixel-map-a-to-a #x0c79)
(define gl:pack-alignment #x0d05)
(define gl:pack-lsb-first #x0d01)
(define gl:pack-row-length #x0d02)
(define gl:pack-skip-pixels #x0d04)
(define gl:pack-skip-rows #x0d03)
(define gl:pack-swap-bytes #x0d00)
(define gl:unpack-alignment #x0cf5)
(define gl:unpack-lsb-first #x0cf1)
(define gl:unpack-row-length #x0cf2)
(define gl:unpack-skip-pixels #x0cf4)
(define gl:unpack-skip-rows #x0cf3)
(define gl:unpack-swap-bytes #x0cf0)
(define gl:zoom-x #x0d16)
(define gl:zoom-y #x0d17)
(define gl:texture-env #x2300)
(define gl:texture-env-mode #x2200)
(define gl:texture-1d #x0de0)
(define gl:texture-2d #x0de1)
(define gl:texture-wrap-s #x2802)
(define gl:texture-wrap-t #x2803)
(define gl:texture-mag-filter #x2800)
(define gl:texture-min-filter #x2801)
(define gl:texture-env-color #x2201)
(define gl:texture-gen-s #x0c60)
(define gl:texture-gen-t #x0c61)
(define gl:texture-gen-r #x0c62)
(define gl:texture-gen-q #x0c63)
(define gl:texture-gen-mode #x2500)
(define gl:texture-border-color #x1004)
(define gl:texture-width #x1000)
(define gl:texture-height #x1001)
(define gl:texture-border #x1005)
(define gl:texture-components #x1003)
(define gl:texture-red-size #x805c)
(define gl:texture-green-size #x805d)
(define gl:texture-blue-size #x805e)
(define gl:texture-alpha-size #x805f)
(define gl:texture-luminance-size #x8060)
(define gl:texture-intensity-size #x8061)
(define gl:nearest-mipmap-nearest #x2700)
(define gl:nearest-mipmap-linear #x2702)
(define gl:linear-mipmap-nearest #x2701)
(define gl:linear-mipmap-linear #x2703)
(define gl:object-linear #x2401)
(define gl:object-plane #x2501)
(define gl:eye-linear #x2400)
(define gl:eye-plane #x2502)
(define gl:sphere-map #x2402)
(define gl:decal #x2101)
(define gl:modulate #x2100)
(define gl:nearest #x2600)
(define gl:repeat #x2901)
(define gl:clamp #x2900)
(define gl:s #x2000)
(define gl:t #x2001)
(define gl:r #x2002)
(define gl:q #x2003)
(define gl:vendor #x1f00)
(define gl:renderer #x1f01)
(define gl:version #x1f02)
(define gl:extensions #x1f03)
(define gl:no-error #x0)
(define gl:invalid-enum #x0500)
(define gl:invalid-value #x0501)
(define gl:invalid-operation #x0502)
(define gl:stack-overflow #x0503)
(define gl:stack-underflow #x0504)
(define gl:out-of-memory #x0505)
(define gl:current-bit #x00000001)
(define gl:point-bit #x00000002)
(define gl:line-bit #x00000004)
(define gl:polygon-bit #x00000008)
(define gl:polygon-stipple-bit #x00000010)
(define gl:pixel-mode-bit #x00000020)
(define gl:lighting-bit #x00000040)
(define gl:fog-bit #x00000080)
(define gl:depth-buffer-bit #x00000100)
(define gl:accum-buffer-bit #x00000200)
(define gl:stencil-buffer-bit #x00000400)
(define gl:viewport-bit #x00000800)
(define gl:transform-bit #x00001000)
(define gl:enable-bit #x00002000)
(define gl:color-buffer-bit #x00004000)
(define gl:hint-bit #x00008000)
(define gl:eval-bit #x00010000)
(define gl:list-bit #x00020000)
(define gl:texture-bit #x00040000)
(define gl:scissor-bit #x00080000)
(define gl:all-attrib-bits #x000fffff)
(define gl:proxy-texture-1d #x8063)
(define gl:proxy-texture-2d #x8064)
(define gl:texture-priority #x8066)
(define gl:texture-resident #x8067)
(define gl:texture-binding-1d #x8068)
(define gl:texture-binding-2d #x8069)
(define gl:texture-internal-format #x1003)
(define gl:alpha4 #x803b)
(define gl:alpha8 #x803c)
(define gl:alpha12 #x803d)
(define gl:alpha16 #x803e)
(define gl:luminance4 #x803f)
(define gl:luminance8 #x8040)
(define gl:luminance12 #x8041)
(define gl:luminance16 #x8042)
(define gl:luminance4-alpha4 #x8043)
(define gl:luminance6-alpha2 #x8044)
(define gl:luminance8-alpha8 #x8045)
(define gl:luminance12-alpha4 #x8046)
(define gl:luminance12-alpha12 #x8047)
(define gl:luminance16-alpha16 #x8048)
(define gl:intensity #x8049)
(define gl:intensity4 #x804a)
(define gl:intensity8 #x804b)
(define gl:intensity12 #x804c)
(define gl:intensity16 #x804d)
(define gl:r3-g3-b2 #x2a10)
(define gl:rgb4 #x804f)
(define gl:rgb5 #x8050)
(define gl:rgb8 #x8051)
(define gl:rgb10 #x8052)
(define gl:rgb12 #x8053)
(define gl:rgb16 #x8054)
(define gl:rgba2 #x8055)
(define gl:rgba4 #x8056)
(define gl:rgb5-a1 #x8057)
(define gl:rgba8 #x8058)
(define gl:rgb10-a2 #x8059)
(define gl:rgba12 #x805a)
(define gl:rgba16 #x805b)
(define gl:client-pixel-store-bit #x00000001)
(define gl:client-vertex-array-bit #x00000002)
(define gl:all-client-attrib-bits #xffffffff)
(define gl:client-all-attrib-bits #xffffffff)
(define gl:rescale-normal #x803a)
(define gl:clamp-to-edge #x812f)
(define gl:max-elements-vertices #x80e8)
(define gl:max-elements-indices #x80e9)
(define gl:bgr #x80e0)
(define gl:bgra #x80e1)
(define gl:unsigned-byte-3-3-2 #x8032)
(define gl:unsigned-byte-2-3-3-rev #x8362)
(define gl:unsigned-short-5-6-5 #x8363)
(define gl:unsigned-short-5-6-5-rev #x8364)
(define gl:unsigned-short-4-4-4-4 #x8033)
(define gl:unsigned-short-4-4-4-4-rev #x8365)
(define gl:unsigned-short-5-5-5-1 #x8034)
(define gl:unsigned-short-1-5-5-5-rev #x8366)
(define gl:unsigned-int-8-8-8-8 #x8035)
(define gl:unsigned-int-8-8-8-8-rev #x8367)
(define gl:unsigned-int-10-10-10-2 #x8036)
(define gl:unsigned-int-2-10-10-10-rev #x8368)
(define gl:light-model-color-control #x81f8)
(define gl:single-color #x81f9)
(define gl:separate-specular-color #x81fa)
(define gl:texture-min-lod #x813a)
(define gl:texture-max-lod #x813b)
(define gl:texture-base-level #x813c)
(define gl:texture-max-level #x813d)
(define gl:smooth-point-size-range #x0b12)
(define gl:smooth-point-size-granularity #x0b13)
(define gl:smooth-line-width-range #x0b22)
(define gl:smooth-line-width-granularity #x0b23)
(define gl:aliased-point-size-range #x846d)
(define gl:aliased-line-width-range #x846e)
(define gl:pack-skip-images #x806b)
(define gl:pack-image-height #x806c)
(define gl:unpack-skip-images #x806d)
(define gl:unpack-image-height #x806e)
(define gl:texture-3d #x806f)
(define gl:proxy-texture-3d #x8070)
(define gl:texture-depth #x8071)
(define gl:texture-wrap-r #x8072)
(define gl:max-3d-texture-size #x8073)
(define gl:texture-binding-3d #x806a)
(define gl:constant-color #x8001)
(define gl:one-minus-constant-color #x8002)
(define gl:constant-alpha #x8003)
(define gl:one-minus-constant-alpha #x8004)
(define gl:color-table #x80d0)
(define gl:post-convolution-color-table #x80d1)
(define gl:post-color-matrix-color-table #x80d2)
(define gl:proxy-color-table #x80d3)
(define gl:proxy-post-convolution-color-table #x80d4)
(define gl:proxy-post-color-matrix-color-table #x80d5)
(define gl:color-table-scale #x80d6)
(define gl:color-table-bias #x80d7)
(define gl:color-table-format #x80d8)
(define gl:color-table-width #x80d9)
(define gl:color-table-red-size #x80da)
(define gl:color-table-green-size #x80db)
(define gl:color-table-blue-size #x80dc)
(define gl:color-table-alpha-size #x80dd)
(define gl:color-table-luminance-size #x80de)
(define gl:color-table-intensity-size #x80df)
(define gl:convolution-1d #x8010)
(define gl:convolution-2d #x8011)
(define gl:separable-2d #x8012)
(define gl:convolution-border-mode #x8013)
(define gl:convolution-filter-scale #x8014)
(define gl:convolution-filter-bias #x8015)
(define gl:reduce #x8016)
(define gl:convolution-format #x8017)
(define gl:convolution-width #x8018)
(define gl:convolution-height #x8019)
(define gl:max-convolution-width #x801a)
(define gl:max-convolution-height #x801b)
(define gl:post-convolution-red-scale #x801c)
(define gl:post-convolution-green-scale #x801d)
(define gl:post-convolution-blue-scale #x801e)
(define gl:post-convolution-alpha-scale #x801f)
(define gl:post-convolution-red-bias #x8020)
(define gl:post-convolution-green-bias #x8021)
(define gl:post-convolution-blue-bias #x8022)
(define gl:post-convolution-alpha-bias #x8023)
(define gl:constant-border #x8151)
(define gl:replicate-border #x8153)
(define gl:convolution-border-color #x8154)
(define gl:color-matrix #x80b1)
(define gl:color-matrix-stack-depth #x80b2)
(define gl:max-color-matrix-stack-depth #x80b3)
(define gl:post-color-matrix-red-scale #x80b4)
(define gl:post-color-matrix-green-scale #x80b5)
(define gl:post-color-matrix-blue-scale #x80b6)
(define gl:post-color-matrix-alpha-scale #x80b7)
(define gl:post-color-matrix-red-bias #x80b8)
(define gl:post-color-matrix-green-bias #x80b9)
(define gl:post-color-matrix-blue-bias #x80ba)
(define gl:post-color-matrix-alpha-bias #x80bb)
(define gl:histogram #x8024)
(define gl:proxy-histogram #x8025)
(define gl:histogram-width #x8026)
(define gl:histogram-format #x8027)
(define gl:histogram-red-size #x8028)
(define gl:histogram-green-size #x8029)
(define gl:histogram-blue-size #x802a)
(define gl:histogram-alpha-size #x802b)
(define gl:histogram-luminance-size #x802c)
(define gl:histogram-sink #x802d)
(define gl:minmax #x802e)
(define gl:minmax-format #x802f)
(define gl:minmax-sink #x8030)
(define gl:table-too-large #x8031)
(define gl:blend-equation #x8009)
(define gl:min #x8007)
(define gl:max #x8008)
(define gl:func-add #x8006)
(define gl:func-subtract #x800a)
(define gl:func-reverse-subtract #x800b)
(define gl:blend-color #x8005)
(define gl:texture0 #x84c0)
(define gl:texture1 #x84c1)
(define gl:texture2 #x84c2)
(define gl:texture3 #x84c3)
(define gl:texture4 #x84c4)
(define gl:texture5 #x84c5)
(define gl:texture6 #x84c6)
(define gl:texture7 #x84c7)
(define gl:texture8 #x84c8)
(define gl:texture9 #x84c9)
(define gl:texture10 #x84ca)
(define gl:texture11 #x84cb)
(define gl:texture12 #x84cc)
(define gl:texture13 #x84cd)
(define gl:texture14 #x84ce)
(define gl:texture15 #x84cf)
(define gl:texture16 #x84d0)
(define gl:texture17 #x84d1)
(define gl:texture18 #x84d2)
(define gl:texture19 #x84d3)
(define gl:texture20 #x84d4)
(define gl:texture21 #x84d5)
(define gl:texture22 #x84d6)
(define gl:texture23 #x84d7)
(define gl:texture24 #x84d8)
(define gl:texture25 #x84d9)
(define gl:texture26 #x84da)
(define gl:texture27 #x84db)
(define gl:texture28 #x84dc)
(define gl:texture29 #x84dd)
(define gl:texture30 #x84de)
(define gl:texture31 #x84df)
(define gl:active-texture #x84e0)
(define gl:client-active-texture #x84e1)
(define gl:max-texture-units #x84e2)
(define gl:normal-map #x8511)
(define gl:reflection-map #x8512)
(define gl:texture-cube-map #x8513)
(define gl:texture-binding-cube-map #x8514)
(define gl:texture-cube-map-positive-x #x8515)
(define gl:texture-cube-map-negative-x #x8516)
(define gl:texture-cube-map-positive-y #x8517)
(define gl:texture-cube-map-negative-y #x8518)
(define gl:texture-cube-map-positive-z #x8519)
(define gl:texture-cube-map-negative-z #x851a)
(define gl:proxy-texture-cube-map #x851b)
(define gl:max-cube-map-texture-size #x851c)
(define gl:compressed-alpha #x84e9)
(define gl:compressed-luminance #x84ea)
(define gl:compressed-luminance-alpha #x84eb)
(define gl:compressed-intensity #x84ec)
(define gl:compressed-rgb #x84ed)
(define gl:compressed-rgba #x84ee)
(define gl:texture-compression-hint #x84ef)
(define gl:texture-compressed-image-size #x86a0)
(define gl:texture-compressed #x86a1)
(define gl:num-compressed-texture-formats #x86a2)
(define gl:compressed-texture-formats #x86a3)
(define gl:multisample #x809d)
(define gl:sample-alpha-to-coverage #x809e)
(define gl:sample-alpha-to-one #x809f)
(define gl:sample-coverage #x80a0)
(define gl:sample-buffers #x80a8)
(define gl:samples #x80a9)
(define gl:sample-coverage-value #x80aa)
(define gl:sample-coverage-invert #x80ab)
(define gl:multisample-bit #x20000000)
(define gl:transpose-modelview-matrix #x84e3)
(define gl:transpose-projection-matrix #x84e4)
(define gl:transpose-texture-matrix #x84e5)
(define gl:transpose-color-matrix #x84e6)
(define gl:combine #x8570)
(define gl:combine-rgb #x8571)
(define gl:combine-alpha #x8572)
(define gl:source0-rgb #x8580)
(define gl:source1-rgb #x8581)
(define gl:source2-rgb #x8582)
(define gl:source0-alpha #x8588)
(define gl:source1-alpha #x8589)
(define gl:source2-alpha #x858a)
(define gl:operand0-rgb #x8590)
(define gl:operand1-rgb #x8591)
(define gl:operand2-rgb #x8592)
(define gl:operand0-alpha #x8598)
(define gl:operand1-alpha #x8599)
(define gl:operand2-alpha #x859a)
(define gl:rgb-scale #x8573)
(define gl:add-signed #x8574)
(define gl:interpolate #x8575)
(define gl:subtract #x84e7)
(define gl:constant #x8576)
(define gl:primary-color #x8577)
(define gl:previous #x8578)
(define gl:dot3-rgb #x86ae)
(define gl:dot3-rgba #x86af)
(define gl:clamp-to-border #x812d)
(define gl:arb-multitexture 1)
(define gl:texture0-arb #x84c0)
(define gl:texture1-arb #x84c1)
(define gl:texture2-arb #x84c2)
(define gl:texture3-arb #x84c3)
(define gl:texture4-arb #x84c4)
(define gl:texture5-arb #x84c5)
(define gl:texture6-arb #x84c6)
(define gl:texture7-arb #x84c7)
(define gl:texture8-arb #x84c8)
(define gl:texture9-arb #x84c9)
(define gl:texture10-arb #x84ca)
(define gl:texture11-arb #x84cb)
(define gl:texture12-arb #x84cc)
(define gl:texture13-arb #x84cd)
(define gl:texture14-arb #x84ce)
(define gl:texture15-arb #x84cf)
(define gl:texture16-arb #x84d0)
(define gl:texture17-arb #x84d1)
(define gl:texture18-arb #x84d2)
(define gl:texture19-arb #x84d3)
(define gl:texture20-arb #x84d4)
(define gl:texture21-arb #x84d5)
(define gl:texture22-arb #x84d6)
(define gl:texture23-arb #x84d7)
(define gl:texture24-arb #x84d8)
(define gl:texture25-arb #x84d9)
(define gl:texture26-arb #x84da)
(define gl:texture27-arb #x84db)
(define gl:texture28-arb #x84dc)
(define gl:texture29-arb #x84dd)
(define gl:texture30-arb #x84de)
(define gl:texture31-arb #x84df)
(define gl:active-texture-arb #x84e0)
(define gl:client-active-texture-arb #x84e1)
(define gl:max-texture-units-arb #x84e2)
(define gl:mesa-shader-debug 1)
(define gl:debug-object-mesa #x8759)
(define gl:debug-print-mesa #x875a)
(define gl:debug-assert-mesa #x875b)
(define gl:mesa-packed-depth-stencil 1)
(define gl:depth-stencil-mesa #x8750)
(define gl:unsigned-int-24-8-mesa #x8751)
(define gl:unsigned-int-8-24-rev-mesa #x8752)
(define gl:unsigned-short-15-1-mesa #x8753)
(define gl:unsigned-short-1-15-rev-mesa #x8754)
(define gl:mesa-program-debug 1)
(define gl:fragment-program-position-mesa #x8bb0)
(define gl:fragment-program-callback-mesa #x8bb1)
(define gl:fragment-program-callback-func-mesa #x8bb2)
(define gl:fragment-program-callback-data-mesa #x8bb3)
(define gl:vertex-program-position-mesa #x8bb4)
(define gl:vertex-program-callback-mesa #x8bb5)
(define gl:vertex-program-callback-func-mesa #x8bb6)
(define gl:vertex-program-callback-data-mesa #x8bb7)
(define gl:mesa-texture-array 1)
(define gl:texture-1d-array-ext #x8c18)
(define gl:proxy-texture-1d-array-ext #x8c19)
(define gl:texture-2d-array-ext #x8c1a)
(define gl:proxy-texture-2d-array-ext #x8c1b)
(define gl:texture-binding-1d-array-ext #x8c1c)
(define gl:texture-binding-2d-array-ext #x8c1d)
(define gl:max-array-texture-layers-ext #x88ff)
(define gl:framebuffer-attachment-texture-layer-ext #x8cd4)
(define gl:ati-blend-equation-separate 1)
(define gl:alpha-blend-equation-ati #x883d)
(define gl:oes-egl-image 1)