#include <iostream>
#include <omp.h>
#include <curl/curl.h>
#include <vector>
#include <chrono>

// Function to handle the response from the server
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    s->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to perform web scraping on a given URL
void scrape_url(const std::string& url, int thread_id, std::vector<std::string>& results) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) {
            std::cerr << "Failed to fetch URL: " << url << " by thread " << thread_id << std::endl;
        } else {
            // Extract the title from the HTML content
            size_t title_start = readBuffer.find("<title>") + 7; // Find the start of the title tag
            size_t title_end = readBuffer.find("</title>");      // Find the end of the title tag

            std::string title = "No title found";
            if (title_start != std::string::npos && title_end != std::string::npos && title_start < title_end) {
                title = readBuffer.substr(title_start, title_end - title_start);
            }

            std::string result = "Data from " + url + ":\nTitle: " + title;
            
            // Use critical section to avoid data races when writing to shared resource
            #pragma omp critical
            {
                results.push_back(result); // Push result into shared vector
            }
        }
    }
}

// Sequential web scraping
void scrape_sequential(const std::vector<std::string>& urls, std::vector<std::string>& results) {
    for (int i = 0; i < urls.size(); ++i) {
        scrape_url(urls[i], -1, results); // -1 means no thread ID in sequential mode
    }
}

// Parallel web scraping using OpenMP
void scrape_parallel(const std::vector<std::string>& urls, std::vector<std::string>& results) {
    int num_urls = urls.size();

    // Set number of threads dynamically
    int max_threads = omp_get_max_threads();
    omp_set_num_threads(max_threads);

    #pragma omp parallel
    {
        #pragma omp single // Ensure only one thread creates tasks
        {
            for (int i = 0; i < num_urls; ++i) {
                #pragma omp task // Create a task for each URL
                {
                    int thread_id = omp_get_thread_num();
                    scrape_url(urls[i], thread_id, results);
                }
            }
        }
        #pragma omp taskwait // Wait for all tasks to complete
    }
}

int main() {
    // List of URLs to scrape
    std::vector<std::string> urls = {
        "https://en.wikipedia.org/wiki/Main_Page",
        "https://openweathermap.org",
        "http://quotes.toscrape.com",
        "http://books.toscrape.com",
        "https://newsapi.org"
    };

    // Sequential scraping
    std::vector<std::string> sequential_results;
    auto start_seq = std::chrono::high_resolution_clock::now();
    scrape_sequential(urls, sequential_results);
    auto end_seq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_seq = end_seq - start_seq;
    std::cout << "Time taken for sequential scraping: " << duration_seq.count() << " seconds.\n";

    // Parallel scraping
    std::vector<std::string> parallel_results;
    auto start_par = std::chrono::high_resolution_clock::now();
    scrape_parallel(urls, parallel_results);
    auto end_par = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_par = end_par - start_par;
    std::cout << "Time taken for parallel scraping: " << duration_par.count() << " seconds.\n";

    // Output comparison
    std::cout << "\nSequential scraping fetched " << sequential_results.size() << " results.\n";
    std::cout << "Parallel scraping fetched " << parallel_results.size() << " results.\n";

    // Output the titles fetched by parallel scraping
    std::cout << "\nTitles fetched from parallel scraping:\n";
    for (const auto& result : parallel_results) {
        std::cout << result << std::endl;
    }

    return 0;
}
