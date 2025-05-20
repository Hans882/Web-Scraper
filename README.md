# Parallel Web Scraper in C++ with OpenMP & cURL
This C++ project demonstrates how to perform **web scraping both sequentially and in parallel** using `libcurl` and `OpenMP`. It fetches webpage titles from a list of URLs and compares the performance between sequential and parallel execution.

## 🚀 Features
  Scrapes the `<title>` tag from a list of URLs  
  Supports both:  
    **Sequential scraping**  
    **Parallel scraping using OpenMP tasks**  
  Measures and compares execution times  
  Handles network errors gracefully  

## Requirements
  C++ compiler supporting C++11 or later (e.g., `g++`, `clang++`)  
  [libcurl](https://curl.se/libcurl/)  
  [OpenMP](https://www.openmp.org/)  

## Installation

### Ubuntu/Debian:
  ```bash
  sudo apt update
  sudo apt install libcurl4-openssl-dev
  ```
### macOS (with Homebrew):
  ```bash
  brew install curl
  ```
### Windows:
  Use MinGW with `libcurl` or build with MSVC and configure dependencies accordingly.

## Compilation

### Using `g++`:
  ```bash
  g++ -fopenmp -lcurl -std=c++11 parallel_scraper.cpp -o scraper
  ```

## Usage
  ```bash
  ./scraper
  ```

## Sample Output
  ```bash
  Time taken for sequential scraping: 3.45612 seconds.
  Time taken for parallel scraping: 1.20387 seconds.
  
  Sequential scraping fetched 5 results.
  Parallel scraping fetched 5 results.
  
  Titles fetched from parallel scraping:
  Data from https://en.wikipedia.org/wiki/Main_Page:
  Title: Wikipedia, the free encyclopedia
  Data from https://openweathermap.org:
  Title: Сurrent weather and forecast
  ...
  ```

## How It Works
 **libcurl** is used to download HTML from each URL.  
The program searches for the content between `<title>` and `</title>` tags.  
**Sequential scraping** processes URLs one by one.  
**Parallel scraping** assigns each URL to a separate OpenMP task.  
A critical section ensures thread-safe access to the shared result vector.   

## Notes
Make sure the URLs are reachable and accessible over your network.  
HTML parsing is minimal — this is not a full-featured scraper.  
