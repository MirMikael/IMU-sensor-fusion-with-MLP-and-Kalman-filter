This code is written to identify a Kalman filter using an artificial neural network <strong>(ANN)</strong>, specifically a multilayer perceptron <strong>(MLP)</strong> responsible for fusing <strong>MPU9250</strong> sensor data <strong>(IMU)</strong>. It's worth mentioning that all components of the MLP, including forward and backward processes, have been manually implemented in this code without utilizing any pre-built functions, solely relying on fundamental commands of the <strong>C</strong> programming language. While the entire ANN training process has been implemented in this code, the initial weights of the MLP have not been randomly selected. This is because the weights used are those of the MLP that have been previously trained in the <strong>MATLAB</strong> simulator for this purpose. In other words, this code is the second stage of NN training, where, unlike the first stage that used random data, real IMU sensor data has been employed.

The general architecture of this MLP is as follows:<br>
<strong>Number of inputs:</strong> 6 neurons (including accelerometer and gyroscope sensor data).<br>
<strong>Hidden layer:</strong> 6 neurons with hyperbolic tangent <strong>(Tanh)</strong> activation function.<br>
<strong>Output layer:</strong> 2 neurons with linear activation function.<br>
The backpropagation <strong>(BP)</strong> algorithm has been utilized to train this NN.<br>
Its optimization function is gradient descent <strong>(GD)</strong>, and its cost function is mean squared error <strong>(MSE)</strong>.<br>

This code is written in <strong>C</strong> language, but due to the requirements of Arduino's framework, some <strong>C++</strong> functions are used in it, but the distinctive features of C++, such as object orientation, are not present in it.<br>
For quastions: <a href="https://mirmikael.github.io/" target="_blank">MirMikael.github.io</a>
