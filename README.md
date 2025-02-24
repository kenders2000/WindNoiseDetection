# Wind Noise Detection in Audio Files
### University Of Salford Acoustics Research Centre

## Overview

This program automatically analyses .wav files and detects regions where there may be microphone wind noise.  Wind noise is known to degrade the quality of outdoor recordings and causes problems with hearing aids.  

This tool could be used when there are a large number of recordings of a particular source, for example clips of recordings from mobile devices of an outdoor concert (user generated content).  This tool can quickly analyse the audio from each to determine samples with the least amount of wind noise and therefore of the highest quality.

The wind noise detection algorithm also provides time stamps identifying the regions which are free of wind noise.  This can allow producers to quickly stitch together content from multiple sources, or flag regions where extra processing such as noise reduction is required.

Perceptual tests have shown that when wind noise is present, the Signal to Noise ratio between the wanted signal and the wind noise is a good indicator of the quality.  Therefore this detector works by first trying to determine the presence of wind noise and then estimating the SNR to provide an indication of the possible degradation to the audio quality.

_See Iain R. Jackson, Paul Kendrick, Trevor J. Cox, Bruno M. Fazenda and Francis Li, "Perceptual evaluation of the functional and aesthetic degradation of speech by wind noise during recording", Proceedings of Meetings on Acoustics, ASA, Vol. 19, 060170 (2013) for further information._


## Usage

The program requires compiling using a C++ compiler and runs as a command line executable which analyses a wavfile and provides information as regards the level, location and effect on quality of any wind noise that may be present.

Wav files must be 16 or 32 bit int, or 32 bit float PCM. All channels are collapsed to one (average of all channels)

```
windDet -i wav_filename -o output_filename -j  output_filename_json[-t 1] [-g 1] [-f 43] [-w 25] [-v 0]
     -i and -o are required parameters, they provide the input .wav filename and the output filename respectively. Specify the output as a text file, include path if required, the program will also output a json file with the same name in the same path.
	 -j specifies the path and name of the json file containing the output data.
     -t n, sets the tree size to n, where n is an integer (1,2 or 3). The default is 2, 1 is fastest but is least accurate.
     -g 1, sets the gain to apply. The default is 1.  If you have calibrated audio you can adjust the level, by default the peak pressure level is assumed to 104 dB, gain can be applied if you have an accurate calibration of the pressure level on the recording, which may increase accuracy but is not expressly required.
     -f n, sets the number of frames used to produce the analysis window to n, windows are 1024 samples long.  The default is n = 43 frames, which is about 1 s
	 -w sets the threshold degradation in quality level for identifying noise free segments, default is 25 (%), but this can be adjusted depending on the required quality level for the application
	 -v tells the program weather or not to print out the terminal output, verbose is 1 or 0, 1 for printout info 0 for no printout.
   
```
   
### Output file description and example

The output text file will contain three sets of outputs: global statistics, wind noise time history, and start and end points of wind noise. Using the following parameter set should provide the results presented.  Additionally a json file will be oututted with the same data


```
windDet.exe -i test/iphone1.wav -o test/iphone1.txt -j test/iphone1.json -t 1 -g 1 -f 43 -w 25 -v 0
```

#### Global statistics about  wind noise

This shows the percentage  of frames which are either free of wind noise or contain wind noise within a particular Signal to (wind)Noise ratio (SNR) range.  These global statistics can be used to make a decision as regards the usability of the file.  There are three rows of numbers. 

 1. 5 ranges of SNRs. 
 2. Corresponding degradation to quality for a particular SNR range.
 3. Percentage of time the signal is within a particular Quality range.
 
The test output of the text file shows an example where 36.4% of frames are free of wind noise, but 54% of the time the quality is degraded by over 50%.  (the json outputs the same data but formatted as to be readable by a json interpreter).


| SNR | >20dB | 20 to 10dB   | 10 to 0dB   | 0 to -10dB | -10 to -20dB   | <-20dB |
|--- | ----- | ---        | ---        | ---        | ---          | ---    |
| **Degradation** | <24%  | 24 to 40   | 40 to 54%   | 54 to 69%    | 69 to 95%  | >96%   |
| **Total time** | 36.4%  | 0.0%       | 18.2%       | 27.3%        | 9.1%         |  9.1%   |

These statistics can be used to make quick judgements about the file regarding the distribution of wind noise levels, but it is down to the required application to choose a suitable set of criteria for acceptability.

#### Wind noise time history

The next set of parameters is the wind noise time history.  Three columns are presented: 

 1. The time at the end of the analysis window
 2. The predicted quality degradation due to wind noise, which is computed from  the predicted SNR
 3. An A-weighted Leq over the window.  
    
The algorithm assumes that a maximum sample value is equivalent to a Sound Pressure Level of 104 dB, [this value was taken as an average of the peak undistorted level of a number of common recording devices](http://usir.salford.ac.uk/29371/1/Distortion_of_Portable_Audio_Devices.pdf). This level can be adjusted by varying the input gain using `-g` should the estimated dBA level appear unrealistic.


| T(s)  | Quality Degradation(%)  | dBA |
| ----- | -------------------------- | --- |
| 1.0 | 0           |  65 |
| 2.0 | 0           |  79 |
| 3.0 | 34            |  82 |
| 4.0 | 0           |  74 |
| 5.0 | 0           |  79 |
| 6.0 | 50            |  88 |
| 7.0 | 66            |  87 |
| 8.0 | 45            |  85 |
| 9.0 | 52            |  87 |
|10.0 | 83            |  87 |
|11.0 | 0           |  82 |

#### Start and end points of wind noise

Finally a set of times is returned which represent the start and end points, in seconds, of regions in the recording which are free of wind noise.  So the following example shows two segments, one from 0 to 1.99s and the next from 2.99 to 4.99 s.

```
0.00  1.99
2.99   4.99
```

An intermediate file is also created which contains the raw classes for sanity checking.

## The Good Recording Project - overview

The University of Salford Acoustics Research Centre is carrying out a project entitled [The Good Recording Project](http://www.goodrecording.net/), investigating how recording errors can affect the perceived quality of audio.  In particular with regards to content recorded on mobile device such as mobile phones, video cameras and other such media often known as "user generated content".  This wind noise detector was developed as part of an [EPSRC funded project](http://gow.epsrc.ac.uk/NGBOViewGrant.aspx?GrantRef=EP/J013013/1).

It is licensed under the MIT license and uses the [KISS_fft library](http://sourceforge.net/projects.kissfft/) which is licensed under the revised BSD license. You are therefore free to use this in pretty much any project.  If you do use this work in any project [an acknowledgement would be appreciated](http://www.salford.ac.uk/computing-science-engineering/subjects/acoustics-audio-and-video).

The software was developed at the University of Salford and uses a bagged decision tree to analyse MFCCs to discover if an audio frame contains microphone wind noise, it is part of the Good Recording Project at the University of Salford Acoustic Research Centre. 


## How does it work?

First the 1024 samples of the wav file are read in, the level calibrated roughly, assuming that a peak sample value is equivalent to 104 dB.  Then 16 Mel Frequency Cepstrum Coefficients (20 to 8000 Hz) are extracted (Hanning weighting), (MFCCs) this is a spectral representation of the frame.  Three sets of frames are collected and first 12 coefficients stored; the first coefficient is replaced with the A-weighted level over the frame in dB.  These 36 features are concatenated and then two ensembles of decision trees take these features as an input and output, a class number.  One tree-ensemble attempts to estimate the wind level:

The Average Wind Level class varies from 0 to 3.

```
0 :  < 30 dBA
1 : 30 to 50 dBA
2 : 50 to 70 dBA
3 : > 70 dBA
```

The second tree-ensemble attempts to estimate the SNR. The average SNR class varies from 0 to 5.

```
0 : < -20 dB   (lots of wind noise!)
1 : -20 to 10dB
2 : -10 to 0dB
3 : 0 to 10 dB
4 : 10 to 20 dB
5 : >  20dB  (no or little wind noise)
```

Class estimates are collected over the analysis period and then averaged, an estimate for quality degradation is computed as follows.  Firstly an analysis window is said to contain wind noise if the average wind level class is greater than 1. If it is less than one, then there is assumed to be no degradation in quality. If it is greater than 1, then the quality is approximated as the result of the following polynomial, where x is the average snr class.

```
0.0093616465*pow(x,9) + -0.2408574076*pow(x,8) + 2.6409491876*pow(x,7) + -16.0598328947*pow(x,6) + 59.1080787296*pow(x,5) + -135.0673590478*pow(x,4) + 189.6816801547*pow(x,3) + -156.8624014598*pow(x,2) + 83.7429623557*pow(x,1) + 4.5226369050;
```

This polynomial maps the average class value to a quality value using the subjective data presented in “Iain R. Jackson, Paul Kendrick, Trevor J. Cox, Bruno M. Fazenda and Francis Li, Perceptual evaluation of the functional and aesthetic degradation of speech by wind noise during recording, Proceedings of Meetings on Acoustics, ASA, Vol. 19, 060170 (2013)”

What is presented is the degradation in quality so 100-quality

For any comments or questions, please email p.kendrick@salford.ac.uk

