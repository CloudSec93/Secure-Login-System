#include <iostream>
#include <random>
#include <chrono>
#include <string>

int main() {

                            
    std::random_device rd;   // generating a high-entropy seed
    unsigned int seed = rd();
    srand(seed);


    int otp = rand() % 900000 + 100000; // range b/w 100000 and 999999

    // for now, since the aws ddb hasnt been created, so i cant really send them emails. Hence for the dry run of the otp gen and validation
    // program, i am simply printing the otp for testing purposes.
    std::cout << "Your OTP is: " << otp << std::endl;


    auto start_time = std::chrono::steady_clock::now();
    // As soon as we print the otp to the user, the timer starts(which currently is for 10 mins)
    // one imp thing in the above code is that we are using steady_clock instead of system_clock meaning that if say some changed the computer's time, our timer wont be affectded. 
    // in monotonoic clock, time always moves forward. if someone manually changes the computer's time(either forward/backward)it will not affect our program timer!


    const int max_attempts = 3;
    const int max_duration = 600; 
    int attempts = 0;
    bool authenticated = false;

    while (attempts < max_attempts) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(
            current_time - start_time
        ).count();

        if (elapsed_seconds >= max_duration) {
            std::cout << "Authentication failed (time expired)" << std::endl;
            break;
        }

        
        // int user_input;
        std::string user_input; // storing the input in a string because there is no guarantee the user will enter only numbers.
        
        std::cout << "Enter the OTP: ";
        std::cin >> user_input;    // new insight: cin function is removing leading and trailing whitespaces

        // once the user enters the password, check if max time hasnt elapsed.
        current_time = std::chrono::steady_clock::now();
        elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(
            current_time - start_time
        ).count();

        if (elapsed_seconds >= max_duration) {
            std::cout << "Authentication failed (time expired)\n";
            break;
        }

        attempts++;

        if (user_input == std::to_string(otp)) {
            std::cout << "Authentication successful!\n";
            authenticated = true;
            break;
        } else {
            std::cout << "Incorrect OTP. You have " << (max_attempts - attempts) << " attempt(s) left.\n"; 
                      
        }
    }

    if (!authenticated && attempts >= max_attempts) {
        std::cout << "Authentication failed (maximum attempts exceeded)\n";
    }

    return 0;
}
