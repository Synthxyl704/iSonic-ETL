# iSonic - audio ETL program
Nothing fancy, just a simple intermediate ETL-type program for converting youtube, soundcloud & more playlists and videos to audio file formats set to your directory. <br>
<hr>

> [!NOTE]
> This program is mostly done with what it is supposed to do, please do message me bugs/any tweaks to make, if I am alive I will definitely respond to you back. <br>
> Some more fun features will still be added as passion features because I honestly had a lot of fun making this. <br>

<hr>

<img width="1921" height="1022" alt="image" src="https://github.com/user-attachments/assets/ae5fd41b-1789-4841-ba46-e337b4edf007" />

<hr>

## RTU - Run Locally

Clone the project using this cmd in bash:
```bash
git clone https://github.com/Synthxyl704/iSonic-ETL
```

CMake guide (simple, execute sequentially if you wish to)
```
cd <directory of storage>
mkdir build
cd build
cmake ..
make
./argvmain
```

Terminal compilation guide (no build directory):
```
cd <directory of storage>
g++ -I. -Iheaders src/argvmain.cpp src/audio_utilities.cpp src/dependency.cpp src/url_utilities.cpp src/file_manager.cpp src/song_manager.cpp src/metadata.cpp src/usage_helper.cpp src/queue_manager.cpp -o main
./main <link_to_youtube_urlL> <audio_file_format>
```

## Commands usage exposition:
1. Downloading a song/music/playlist -
```
./main <link to the video/music/playlist> <format> <(optional) custom folder/directory>
```
if the `argv[3]` is not specified, it will auto create a "downloads" named folder and download it there, wont just download anywhere or in the program itself.

2. Listing downloaded songs/music -
```
./main -smlist
```

3. Deleting a downloaded song/music piece from terminal itself -
```
./main -rem
```

4. Wanna see song metadata without filling up your terminal screen?
```
./main -metamsc
```

5. Folder/directory metadata (space allocation, usage, file formats listing, etc)
```
./main -LFI_t <folder/directory> // THIS WILL SHOW DETAILED INFO
./main -LFI_f <folder/directory> // THIS WILL NOT SHOW DETAILED INFO
```

6. Remove multiple files in one command
```
./main -remMul <index_1> <index_2> ... <index_n>
```

7. Transcode existing music files (example: XXX.mp3 to XXX.opus with actual codec change)
```
./main -transcode <index_1>, <index_2>, <index_n>
```

8. Search for music files (parses tokens seperately so that you can get all files with even one analysed or matching token in them)
```
./main -search tokenOne_tokenTwo_tokenThree
```

<hr>

## UPD/RR logs:

-> 02/09/25 [FIXED] | `-smlist` listed dates may be incorrect at times for certain/random songs. [ !Highly unlikely! ] <br>
-> `-LFI_t` is restricted to only showing you the detailed version of directory metadata, anything else that isnt 't' will automatically show you the non-detailed version.

## P-TODO: <br> 
-> Maybe improve aesthetics on menu, idk how to do that though

## License

[CC 4.0]([https://creativecommons.org/licenses/by/4.0/deed.en)

<h3>IsoAris -REVITALIZE-</h3>
