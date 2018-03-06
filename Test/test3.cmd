if "10.77.7.19" EQU "" goto error

tskill ffmpeg
start /min ffmpeg -re -i ../Debug/telephone_call_from_istanbul.mp3 -vn  -f rtp rtp://10.77.7.19:29000?localport=40000
start /min ffmpeg -re -i ../Debug/ill_be_gone.mp3 -vn  -f rtp rtp://10.77.7.19:29002?localport=40002
start /min ffmpeg -re -i ../Debug/kalimba.mp3 -vn  -f rtp rtp://10.77.7.19:29004?localport=40004
start /min ffmpeg -re -i ../Debug/track(4).wav -vn  -f rtp rtp://10.77.7.19:29006?localport=40006
exit

:error
echo Usage: test.cmd <Destination IP>
exit