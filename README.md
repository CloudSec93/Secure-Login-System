
This project is a Secure Login System and Password Manager developed in C++ that focusses on both authentication security and local password management. 

Login System: Users authenticate via a password-based login. On each login attempt, a One-Time Password (OTP) is sent to their registered email using AWS SES.
  Security Features:

  Passwords are hashed using Argon2 (or a custom hash function, based on user choice).
  Passwords are securely stored on AWS DynamoDB, with the user ID as the primary key and email as a Global Secondary Index (GSI).
  The system enforces a maximum of 3 failed login attempts before initiating a 5-minute lockout.
  
  Password Manager:

  Passwords for various services are stored locally.
  Users can choose between Caesar Cipher or Vigenère Cipher to encrypt their saved passwords.

Tech Stack: C++, AWS DynamoDB, AWS SES
