This code is written to identify a Kalman filter using an artificial neural network (ANN), specifically a multilayer perceptron (MLP) responsible for fusing MPU9250 sensor data (IMU). It's worth mentioning that all components of the MLP, including forward and backward processes, have been manually implemented in this code without utilizing any pre-built functions, solely relying on fundamental commands of the C programming language. While the entire ANN training process has been implemented in this code, the initial weights of the MLP have not been randomly selected. This is because the weights used are those of the MLP that have been previously trained in the MATLAB simulator for this purpose. In other words, this code is the second stage of NN training, where, unlike the first stage that used random data, real IMU sensor data has been employed.

The general architecture of this MLP is as follows:<br>
Number of inputs: 6 neurons (including accelerometer and gyroscope sensor data).<br>
Hidden layer: 6 neurons with hyperbolic tangent (Tanh) activation function.<br>
Output layer: 2 neurons with linear activation function.<br>
The backpropagation (BP) algorithm has been utilized to train this NN.<br>
Its optimization function is gradient descent (GD), and its cost function is mean squared error (MSE).<br>

This code is written in C language, but due to the requirements of Arduino's framework, some C++ functions are used in it, but the distinctive features of C Plus Plus, such as object orientation, are not present in it.
