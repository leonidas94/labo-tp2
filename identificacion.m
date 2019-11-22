close all

Tventana=0.01;
duty_cycle=200;
%lazo abierto
 for i=1:length(in.signals.values);
     salida(i)=output.signals.values(:,:,i);
 end
order=[ 1,1,2];
data=iddata(salida',in.signals.values,Tventana) % trabajo los datos del simulink
sys=arx(data,order)
figure()
opt=stepDataOptions('StepAmplitude',duty_cycle);
hold on
plot(output.time,salida,'r')
step (sys,opt) %Respuesta al escalon para comparar con el Simulink
legend( 'respuesta real','respuesta del sistema equivalente')
tf(sys) % Para ver la transferencia
% para llevarla al simulink para hacer el punto 3
[num,dem]=tfdata(sys);
num=num{1};
dem=dem{1};
grid on
