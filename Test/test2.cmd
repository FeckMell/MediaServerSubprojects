if "10.77.7.19" EQU "" goto error

tskill ffmpeg
start /min ffmpeg -re -i ../Debug/output0.wav -vn  -f rtp rtp://10.77.7.19:29004
start /min ffmpeg -re -i ../Debug/output1.wav -vn  -f rtp rtp://10.77.7.19:29002
start /min ffmpeg -re -i ../Debug/output2.wav -vn  -f rtp rtp://10.77.7.19:29000
exit

:error
echo Usage: test.cmd <Destination IP>
exit