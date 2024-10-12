#include <iostream>
#include <curl/curl.h>
#include "json.hpp" // Include nlohmann/json for JSON handling
#include <vector>
#include <algorithm> // for random_shuffle
#include <ctime>     // for srand()

using json = nlohmann::json;
using namespace std;

// Function to handle API response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* s) {
    s->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to make API call and fetch quiz questions
json fetchQuizQuestionsFromAPI(const string& url) {
    CURL* curl;
    CURLcode res;
    string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) {
            cerr << "Failed to fetch questions from API: " << curl_easy_strerror(res) << endl;
            exit(1);
        }
    }

    // Parse the JSON response
    json questionsJson = json::parse(readBuffer);
    return questionsJson;
}

// Function to ask questions and calculate the score
int askQuestionsAndCalculateScore(const json& questions) {
    int score = 0;
    int questionNumber = 1;

    for (const auto& question : questions["results"]) {
        cout << "Q" << questionNumber++ << ": " << question["question"] << endl;

        vector<string> options = question["incorrect_answers"];
        options.push_back(question["correct_answer"]);

        // Shuffle the options to randomize their order
        srand(time(0));
        random_shuffle(options.begin(), options.end());

        // Display options
        char optionLabel = 'A';
        map<char, string> optionMap;
        for (const auto& option : options) {
            cout << optionLabel << ". " << option << endl;
            optionMap[optionLabel] = option;
            optionLabel++;
        }

        // Get user's answer
        char userAnswer;
        cout << "Enter your choice (A/B/C/D): ";
        cin >> userAnswer;
        userAnswer = toupper(userAnswer);

        // Check if the answer is correct
        if (optionMap[userAnswer] == question["correct_answer"]) {
            cout << "Correct!" << endl;
            score++;
        } else {
            cout << "Wrong! The correct answer was: " << question["correct_answer"] << endl;
        }

        cout << endl; // Line break for better readability
    }

    return score;
}

int main() {
    int num;
      cout<<"Enter the number of questions required !"<<endl;
   cin>>num;
    string apiUrl = "https://opentdb.com/api.php?amount={num}"; // API URL
 
    // Fetch quiz questions from the API
    json quizQuestions = fetchQuizQuestionsFromAPI(apiUrl);

    // Ask questions and calculate the score
    int totalQuestions = quizQuestions["results"].size();
    int score = askQuestionsAndCalculateScore(quizQuestions);

    // Display final score
    cout << "Quiz over! You scored " << score << " out of " << totalQuestions << "!" << endl;

    return 0;
}
