Tw=0.01;
ref=4875;

 for i=1:length(in.signals.values);
     salida(i)=output.signals.values(:,:,i);
 end
 
order=[1,1,2];
data=iddata(salida',in.signals.values,Tw)
sys=arx(data,order)

figure()
opt=stepDataOptions('StepAmplitude',ref);
hold on
plot(output.time,salida,'r')
step (sys,opt) %Respuesta al escalon para comparar con el Simulink
legend('respuesta real','respuesta del sistema equivalente', 'location', 'southeast')
grid on
tf(sys) % Para ver la transferencia

[num,den]=tfdata(sys);
num=num{1};
den=den{1};