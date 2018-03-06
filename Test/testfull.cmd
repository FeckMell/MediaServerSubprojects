if "10.77.7.19" EQU "" goto error

tskill ffmpeg
start /min ffmpeg -re -i ../Debug/music.wav -vn -f rtp rtp://10.77.7.19:10000
start /min ffmpeg -re -i ../Debug/telephone_call_from_istanbul.mp3 -vn  -f rtp rtp://10.77.7.19:20000
start /min ffmpeg -re -i ../Debug/ill_be_gone.mp3 -vn  -f rtp rtp://10.77.7.19:30000
start /min ffmpeg -re -i ../Debug/kalimba.mp3 -vn  -f rtp rtp://10.77.7.19:40000
exit

:error
echo Usage: test.cmd <Destination IP>
exit