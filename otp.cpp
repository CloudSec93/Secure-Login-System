// otp.cpp
#include "otp.h"

#include <aws/email/SESClient.h>
#include <aws/email/model/SendEmailRequest.h>
#include <aws/core/utils/Outcome.h>
#include <iostream>
#include <random>
#include <chrono>
#include <string>

bool sendOTPEmail(const std::string& email, const std::string& otp) {
    Aws::Client::ClientConfiguration clientConfig;
    Aws::SES::SESClient sesClient(clientConfig);

    Aws::SES::Model::SendEmailRequest sendEmailRequest;
    Aws::SES::Model::Destination destination;
    destination.AddToAddresses(email.c_str());

    sendEmailRequest.SetDestination(destination);

    Aws::SES::Model::Message message;
    Aws::SES::Model::Content subject;
    subject.SetData("Your OTP Code");

    Aws::SES::Model::Content bodyContent;
    std::string bodyText = "Your OTP is: " + otp;
    bodyContent.SetData(bodyText.c_str());

    Aws::SES::Model::Body body;
    body.SetText(bodyContent);

    message.SetSubject(subject);
    message.SetBody(body);

    sendEmailRequest.SetMessage(message);

    sendEmailRequest.SetSource("harsh.maheshwary@flame.edu.in");

    auto sendEmailOutcome = sesClient.SendEmail(sendEmailRequest);

    if (!sendEmailOutcome.IsSuccess()) {
        std::cerr << "Failed to send email: " 
                  << sendEmailOutcome.GetError().GetMessage() << std::endl;
        return false;
    }

    return true;
}

bool validateOTP(const std::string& email) {
    std::random_device rd;
    unsigned int seed = rd();
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(100000, 999999);
    int otp = dist(rng);

    std::string otp_str = std::to_string(otp);

    if (!sendOTPEmail(email, otp_str)) {
        std::cerr << "Error: Could not send OTP email." << std::endl;
        return false;
    }

    auto start_time = std::chrono::steady_clock::now();

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
            std::cout << "Authentication failed: time expired." << std::endl;
            break;
        }

        std::string user_input;
        std::cout << "Enter the OTP sent to your email: ";
        // std::cin >> user_input;
        std::getline(std::cin, user_input);

        current_time = std::chrono::steady_clock::now();
        elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(
            current_time - start_time
        ).count();

        if (elapsed_seconds >= max_duration) {
            std::cout << "Authentication failed: time expired." << std::endl;
            break;
        }

        attempts++;

        if (user_input == otp_str) {
            std::cout << "Authentication successful!" << std::endl;
            authenticated = true;
            break;
        } else {
            std::cout << "Incorrect OTP. You have " << (max_attempts - attempts) << " attempt(s) left." 
                      << std::endl;
        }
    }

    if (!authenticated && attempts >= max_attempts) {
        std::cout << "Authentication failed: maximum attempts exceeded." << std::endl;
    }

    return authenticated;
}






