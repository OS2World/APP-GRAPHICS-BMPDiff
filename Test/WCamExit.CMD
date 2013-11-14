/* Test BMPDiff */

/* AlarmSound = "D:\MMOS2\SOUNDS\st8bit\NIXZUSU.WAV" */
AlarmSound = "D:\MMOS2\SOUNDS\TWIP.WAV"

    '@BMPDiff -1:WEBCAM -2:Old -b:7'

    /* Send RC to program QOut.CMD if it is active and Q was created      */
    QName = "QBMPDIFF"                      /* Output RC to QOut.CMD      */
    NQName = RXQUEUE("CREATE", QName)       /* Test for existence of Q    */
    if NQName \= QName then do              /* Q 'QBMPDIFF' exist already */
        call RXQUEUE "DELETE", NQName       /* Kill new created Q         */
        call RXQUEUE "SET", QName           /* Use QBMPDIFF               */
        QUEUE RC                            /* Output to Q                */
    end

    if (RC > 200) then do                   /* Movement in Picture ? */

        /* Play a WAV FIle */
        call RXFUNCADD 'mciRxInit','MCIAPI','mciRxInit'
        call mciRxInit
        call mciRxSendString "open" AlarmSound "alias wave wait", 'RetStr', '0', '0'
        call mciRxSendString "play wave wait", 'RetStr', '0', '0'
        call mciRxSendString "close wave wait", 'RetStr', '0', '0'
        call mciRxExit

        /* Save the Picture */
        today = date('S')                       /* Directory name is the date */
        zeit  = translate(time('N'),'_',':')    /* Filename is the time       */
        '@md' today
        '@copy WEBCAM.BMP' today'\'zeit'.BMP'
        '@del WEBCAM.BMP'                       /* Delete all BMP's */
        '@del Old.BMP'

        end

    else do                                     /* No change in Picture */
        '@copy WEBCAM.BMP Old.BMP'
        end

    exit(0)

