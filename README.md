# RGB-YUV-Converter
### Тестовое задание
![Image](task.png)
### Результаты
![Results](results.png)
### Сборка и конвертация
```
cd build && cmake ..
make
```
Конвертация
```
./converter samples/flower.yuv 352 288 samples/House.bmp samples/output.yuv
```
Воспроизведение .yuv c помощью mplayer
```
mplayer -demuxer rawvideo -rawvideo w=352:h=288 samples/output.yuv
```
