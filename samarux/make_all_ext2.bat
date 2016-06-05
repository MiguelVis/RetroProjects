@echo off
rem Make all the SamaruX external commands from the built-in ones
rem Usage: make_all_ext2
call make_ext2 sx_ascii ascii
call make_ext2 sx_cat cat
call make_ext2 sx_chmod chmod
call make_ext2 sx_clear clear
call make_ext2 sx_cp cp
call make_ext2 sx_cpm cpm
call make_ext2 sx_date date
call make_ext2 sx_df df
call make_ext2 sx_diral diralias
call make_ext2 sx_dump dump
call make_ext2 sx_echo echo
call make_ext2 sx_ed ed
call make_ext2 sx_grep grep
call make_ext2 sx_ls ls
call make_ext2 sx_man man
call make_ext2 sx_more more
call make_ext2 sx_mv mv
call make_ext2 sx_rm rm
call make_ext2 sx_sort sort
call make_ext2 sx_tail tail
call make_ext2 sx_tee tee
call make_ext2 sx_ver ver
call make_ext2 sx_wc wc
call make_ext2 sx_whoam whoami
echo.
echo -------------
echo Build results
echo -------------
echo.
call make_tst ascii.x
call make_tst cat.x
call make_tst chmod.x
call make_tst clear.x
call make_tst cp.x
call make_tst cpm.x
call make_tst date.x
call make_tst df.x
call make_tst diralias.x
call make_tst dump.x
call make_tst echo.x
call make_tst ed.x
call make_tst grep.x
call make_tst ls.x
call make_tst man.x
call make_tst more.x
call make_tst mv.x
call make_tst rm.x
call make_tst sort.x
call make_tst tail.x
call make_tst tee.x
call make_tst ver.x
call make_tst wc.x
call make_tst whoami.x
echo.