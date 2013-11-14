/* QOut.CMD */

    call rxfuncadd "SysSleep", "rexxutil", "SysSleep"

    QName = "QBMPDIFF"
    rc = RXQUEUE("DELETE", QName)   /* Kill eventually existing Q */
    rc = RXQUEUE("CREATE", QName)   /* Create Q */

    rc = RXQUEUE("SET", QName)      /* Use QBMPDIFF */
    say "Q is active"

    do forever
        if QUEUED() > 0 then do
            bmpdiff_rc = linein("QUEUE:")
            if bmpdiff_rc > 0 then
                say date() time() "Image difference:" bmpdiff_rc
            else
                say date() time() "BMPDIFF RC:      " bmpdiff_rc
            end
        call SysSleep(1)
    end /* do */


