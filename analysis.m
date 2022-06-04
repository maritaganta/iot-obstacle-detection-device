% Read the Obstacle detected Status channel data.
% Send an email and tell the user that obstacle is detected if the value
% is equal to 0.
% Store the channel ID for the Obstacle detection channel.
channelID = 1675318;
% Provide the ThingSpeak alerts API key. All alerts API keys start with TAK.
alertApiKey = 'TAKJMY6ENQCK6QQMT923C';
% Set the address for the HTTTP call
alertUrl="https://api.thingspeak.com/alerts/send";

% webwrite uses weboptions to add required headers. Alerts needs a ThingSpeak-Alerts-
API-Key header.

options = weboptions("HeaderFields", ["ThingSpeak-Alerts-API-Key", alertApiKey ]);
% Set the email subject.
alertSubject = sprintf("Obstacle information");
% Read the recent data.
%obstacleData = thingSpeakRead(channelID,'NumDays',30,'Fields',1);
obstacleData = thingSpeakRead(channelID,'Fields',1);
% Check to make sure the data was read correctly from the channel.
if isempty(obstacleData)
alertBody = ' No data read regarding Obstacle. ';
else
% Calculate a 10% threshold value based on recent data.
span = max(obstacleData) - min(obstacleData);
% Get the most recent point in the array of obstacleData data.
lastValue = obstacleData(end);
disp(lastValue);
% Set the outgoing message
if (lastValue > 0)
alertBody = ' Obstacle detected. ';
disp('Obstacle detected')
elseif (lastValue <= 0)
alertBody = ' No Obstacle. ';
disp('No Obstacle')
end
end
% Catch errors so the MATLAB code does not disable a TimeControl if it fails
try
webwrite(alertUrl , "body", alertBody, "subject", alertSubject, options);
catch someException
fprintf("Failed to send alert: %s\n", someException.message);
end
