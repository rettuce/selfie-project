#pragma once

//#define MODE_DEBBUG
#define USE_KINECT

#ifdef MODE_DEBBUG
#else
#endif

const unsigned int WIDTH = 1920;
const unsigned int HEIGHT = 1080;

const unsigned int OUT_GIFW = 600;
const unsigned int OUT_GIFH = 600;


#ifdef MODE_DEBBUG
const string HOST = "http://selfie.rettuce.test.com/";  // local
#else
const string HOST = "http://selfie.rettuce.com/";
#endif

const string API_GIF = "oF/upload_gif.php";
const string API_PNG = "oF/upload_png.php";
