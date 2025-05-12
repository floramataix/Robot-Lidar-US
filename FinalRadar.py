import re
import matplotlib
matplotlib.use('Qt5Agg')  # Utilisation de Qt5 pour l'affichage
import numpy as np
from matplotlib import pyplot as plt
import serial
import serial.tools.list_ports
import keyboard
import time

# === Configuration ===
r_max = 200            # Distance maximale affichée (en cm)
refresh_rate = 0.1     # Taux de rafraîchissement (en secondes)

# === Choix du port série ===
ports = [port.device for port in serial.tools.list_ports.comports()]
if not ports:
    print("Aucun port série détecté. Vérifiez votre connexion USB.")
    exit()

print("Ports disponibles :")
for i, p in enumerate(ports):
    print(f"{i + 1}. {p}")

serial_port = ports[0]
print(f"Sélection automatique du port: {serial_port}")

# === Initialisation du graphique ===
fig = plt.figure(facecolor='k', figsize=(10, 8))
fig.canvas.manager.set_window_title('Radar Ultrasonique - 8 Capteurs')
ax = fig.add_subplot(111, polar=True, facecolor='#006b70')
ax.set_theta_zero_location('N')
ax.set_theta_direction(-1)
ax.set_ylim(0, r_max)
ax.set_xlim(0, 2*np.pi)
ax.grid(color='white', linestyle='--', alpha=0.5)

angles = np.deg2rad(np.arange(0, 360, 45))
dists = np.zeros(8)

scatter = ax.scatter(angles, dists, c='red', s=100, alpha=0.7, edgecolors='white')
line = ax.plot([], [], 'w-', linewidth=2)[0]

for i, angle in enumerate(np.rad2deg(angles)):
    ax.text(angles[i], r_max + 10, f'C{i}', ha='center', va='center', color='white')

plt.tight_layout()
plt.show(block=False)

# === Préparation du parsing par regex ===
# Exemple de ligne attendue : "C0:125 C1:200 ... C7:50"
sensor_pattern = re.compile(
    r'Capteur\s*(\d+)\s*:\s*([0-9]+(?:\.[0-9]*)?)\s*cm'
)
# === Connexion série ===
try:
    ser = serial.Serial(serial_port, 9600, timeout=2)
    print(f"Connecté à {serial_port}")
    last_update = time.time()

    while True:
        # Lecture non bloquante
        if ser.in_waiting:
            raw = ser.readline().decode('utf-8', errors='ignore').strip()

            if raw.startswith('C'):
                # On cherche toutes les occurrences Cn:valeur
                matches = sensor_pattern.findall(raw)
                if matches:
                    for snum, sval in matches:
                        idx = int(snum)
                        dist = float(sval)
                        dists[idx] = min(dist, r_max)

                    # Mise à jour graphique si nécessaire
                    if time.time() - last_update >= refresh_rate:
                        scatter.set_offsets(np.column_stack((angles, dists)))
                        closest_idx = np.argmin(dists)
                        line.set_data(
                            [angles[closest_idx]] * 2,
                            [0, dists[closest_idx]]
                        )
                        fig.canvas.draw()
                        fig.canvas.flush_events()
                        last_update = time.time()
                else:
                    print(f"Ligne non reconnue : {raw}")

        # Arrêt si 'q' pressée
        if keyboard.is_pressed('q'):
            print("Arrêt demandé par l'utilisateur")
            break

except serial.SerialException as e:
    print(f"Erreur port série: {e}")
except KeyboardInterrupt:
    print("Arrêt par Ctrl+C")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
    plt.close()
    print("Port série fermé")
