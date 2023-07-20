# scenarios

- <code>0_scenarios.R</code> generates different geographic vaccination strategies scenarios (<code>scenarios_csv/scenarios_N.csv</code>) based on three parameters ($\Delta p$, $\Delta n$, $\Delta t$). 
- $\Delta p$ (in the code: <code>Regional</code>): The proportion of extra vaccine doses provided to the Plus group. 
- $\Delta n$ (in the code: <code>Geo</code>): The shift of municipality priority towards the Plus group ($\Delta n > 0$) or Minus group ($\Delta n < 0$). 
- $\Delta t$ (in the code: <code>Regional_start</code>): The timing of the start of the vaccination program. 
- Other columns in the file are non-relevant and used in other studies. 
