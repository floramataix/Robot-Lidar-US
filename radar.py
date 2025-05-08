import matplotlib
matplotlib.use('Qt5Agg')  # Utilisation de Qt5 pour l'affichage
from matplotlib import pyplot as plt
import numpy as np
import serial
import keyboard

# Initialisation du graphique
fig = plt.figure(facecolor='k')

# Masquer la barre d'outils avec Qt5 en définissant le manager
fig.canvas.manager.toolbar.hide()  # Pour Qt5Agg, on utilise `.hide()` pour masquer la barre d'outils

fig.canvas.manager.set_window_title('Ultrasonic Radar')

# Plein écran avec Qt5
mgn = plt.get_current_fig_manager()
mgn.resize(800, 600)  # ✅ Fenêtre de 800x600 pixels

# Création du graphique en coordonnées polaires
ax = fig.add_subplot(1, 1, 1, polar=True, facecolor='#006b70')
ax.tick_params(axis='both', colors='w')

r_max = 100
ax.set_ylim([0.0, r_max])
ax.set_xlim([0.0, 2 * np.pi])
ax.set_position([-0.05, -0.05, 1.1, 1.05])
ax.set_rticks(np.linspace(0.0, r_max, 5))
ax.set_thetagrids(np.linspace(0.0, 360, 9))

# Initialisation des angles et des données
angles = np.arange(0, 361, 1)
theta = angles * (np.pi / 180)

# Création des objets graphiques
pols, = ax.plot([], [], linestyle='', marker='o', markerfacecolor='r',
                markeredgecolor='w', markeredgewidth=1.0, markersize=3.0, alpha=0.5)
line1, = ax.plot([], [], color='w', linewidth=3.0)

fig.canvas.draw()

# Copie de l'arrière-plan pour une mise à jour rapide
dists = np.ones(len(angles))
axbackground = fig.canvas.copy_from_bbox(ax.bbox)  # ✅ Sauvegarde de l'arrière-plan

# Détection du port série disponible
def detect_serial_port():
    import serial.tools.list_ports
    ports = [port.device for port in serial.tools.list_ports.comports()]
    return ports[0] if ports else None

# Trouver un port valide
serial_port = detect_serial_port()
if serial_port is None:
    print("Aucun port série détecté. Vérifiez votre connexion USB.")
    exit()

# Ouverture sécurisée du port série
with serial.Serial(serial_port, baudrate=9600, bytesize=8, parity='N', stopbits=1, timeout=2) as ser:
    print(f"Connexion établie avec {serial_port}")

    while True:
        try:
            # Lecture et décodage des données série
            data = ser.readline().decode("utf-8", errors="ignore").strip()

            # Vérification des données reçues
            if not data or ',' not in data:
                continue

            try:
                angle, dist = map(float, data.split(','))
            except ValueError:
                continue  # Ignore les données mal formatées

            # Mise à jour des données
            dists[int(angle) % 360] = dist
            pols.set_data(theta, dists)

            # Restaurer l'arrière-plan pour un affichage fluide
            fig.canvas.restore_region(axbackground)

            # Dessiner les objets mis à jour
            ax.draw_artist(pols)
            line1.set_data([angle * (np.pi / 180)] * 2, np.linspace(0.0, r_max, 2))
            ax.draw_artist(line1)

            # Mise à jour de l'affichage
            fig.canvas.blit(ax.bbox)
            fig.canvas.flush_events()

            # Quitter l'application si l'utilisateur appuie sur 'q'
            if keyboard.is_pressed('q'):
                plt.close('all')
                print("L'utilisateur a quitté l'application")
                break

        except KeyboardInterrupt:
            plt.close('all')
            print('Interruption clavier - Fermeture du programme')
            break

print("Port série fermé proprement.")
plt.show()
