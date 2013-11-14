/* Test BMPDiff */

DO forever
    '@remote'
     call SysSleep(1)
    '@copy WEBCAM.BMP Image1.BMP > NUL'
    '@remote'
     call SysSleep(1)
    '@copy WEBCAM.BMP Image2.BMP > NUL'
    '@BMPDiff -1:Image1 -2:Image2 -d:Diff -b:0  > NUL'
    say RC
    if (RC > 100) THEN call play ("FILE=E::\automotion\bmpdiff\test1\beeoong.wav")
    call SysSleep(3)

end /* do */
