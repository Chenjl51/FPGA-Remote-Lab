#1. update system
sudo apt-get update
sudo apt-get upgrade

#2. copy master.zip under the catalogue of pi

#3. unzip
unzip master.zip

#4. install cmake, used for compile
sudo apt-get install cmake -y

#5. install support library
sudo apt-get install libjpeg8-dev -y

#6. compile
sudo make clean all

#7. reboot system
sudo reboot

#8. enter system
cd /home/pi/mjpg-streamer-master/mjpg-streamer-experimental/

#9. enable usb camera
./mjpg_streamer -i "./input_uvc.so" -o "./output_http.so -w ./www" 

#10. online test, which xxx.xxx.x.xxx is the ip of camera
http://xxx.xxx.x.xxx:8080/?action=snapshot  

#11. using this camera will occupy the resource, it must be killed after use
ps a
sudo kill -9 PID
