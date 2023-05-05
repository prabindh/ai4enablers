# ai4enablers
##AI tools for enablers

AI tools for enablers working in various content conversion fields that are accessible to all.

##Some examples:

1. An organisation working to convert printed books to Braille format using OCR. The first step involves scanning the printed book to PDF, followed by the step of OCR that converts the PDF to an editable MS-word document using OCR. 

One of the biggest real challenges is - the software used to convert scanned pages to MS-word, does not work well for pages with tables, images. A lot of manual work is involved in then locating these pages in the document, then manually converting them to editable-tables in Word.

#Project 1 - Tabler

A simple OCR tool to locate pages in PDF files, with tables. Lot of heuristic code.

##Pre-requirements:

1. OS - Windows, Currently tested only on Windows. It can be made to work on other OS with a little bit of modifications.

2. OpenCV

Set OPENCV_DIR env variable to the complete path to the below folder

opencv\build

ie, the below folder should be available for linking

{OPENCV_DIR}\x64\vc15\bin

3. Qt5 / Qt creator

###How to build:

- Open the Qt Makefile from tabler\CMakeLists.txt, and build the project.
- If all succeeds, it will generate tabler.exe

Run tabler and select an input PDF file.

![Screenshot](tabler/screen1.jpg)

##Related:

- https://github.com/prabindh/viola

# Project 2 - Vision sign language reading

For ex, a good starting point using TF Lite - https://www.kaggle.com/competitions/asl-signs

