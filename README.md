# Modeling geographic vaccination strategies for COVID-19 in Norway

[![License](https://img.shields.io/badge/license-YourLicense-brightgreen)](LICENSE)
[![GitHub stars](https://img.shields.io/github/stars/imlouischan/corona-no)](https://github.com/imlouischan/corona-no/stargazers)

## Descriptions

This GitHub repository (https://github.com/imlouischan/corona-no) contains the codes of the individual-based model (IBM) for simulating vaccination strategies for COVID-19 in Norway. 
For the codes of the meta-population model (MPM), please refer to another GitHub repository (https://github.com/Gulfa/regional_vaccination). 

## Contents

### 0-pre-processing

#### case_data
- This folder contains data of confirmed cases, hospital admissions, ICU admissions, and deaths for calibrating the IBM (and MPM). 

#### initial_I
- This folder contains initial values for the IBM (and MPM) using outputs from the situational awareness and forecasting model. 

#### parameters
- This folder contains parameters of the IBM (and MPM), including transmission and hospitalization. 

#### scenarios
- This folder contains different geographic vaccination strategies scenarios for the IBM (and MPM). 

### 1-main-ABM

#### In_out
- This folder contains input and output files for the IBM. 

#### src
- This folder contains the main codes for the IBM to simulate COVID-19 transmission in Norway. 
