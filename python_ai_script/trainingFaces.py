# FACE RECOGNITION: acquisizione volti
# 1. Acquisisce le immagini (frame) da videocamera
# 2. Converte il singolo frame da BGR (RGB) in intervalli di grigio
# 3. Imposta l'algoritmo da usare nel riconoscimento (frontalface)
# 4. Per ogni viso individuato: disegna una cornice intorno e
#    lo salva nella cartella apposita.

# importa la libreria opencv (package opencv-contrib-python)
import cv2

# importa la libreria per la gestione di file e cartelle
import os

# acquisisce il nome del volto
face_name=input('Inserisci il nome della persona: ')
output_folder = './faces-db/'+face_name
print('output_folder: '+output_folder)

# se non esiste la cartella desiderata, la crea
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

#https://github.com/opencv/opencv/tree/master/data/haarcascades
#print('Haarcascade: '+cv2.data.haarcascades+'haarcascade_frontalface_default.xml')

# carica gli algoritmi di riconoscimento
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades+'haarcascade_frontalface_default.xml')
#face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades+'haarcascade_eye.xml')

# attiva la cattura da videocamera
camera = cv2.VideoCapture(0)
count = 0

# waitKey(1) aspetta la pressione di un tasto qualsiasi e visualizza un frame ogni 1 ms
while (cv2.waitKey(1) == -1):
    # legge il frame dall videocamera
    success, frame = camera.read()

    if success:

        # applica il filtro a livelli di grigio al frame
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # esegue face recognition sul frame e ritorna le coordinate e le dimensioni del riquadro
        faces = face_cascade.detectMultiScale(gray, 1.3, 5, minSize=(120, 120))

        # ritaglia il riquadro del volto dal frame e disegna la cornice colorata
        for (x, y, w, h) in faces:
            # disegna un rettangolo nel frame letto
            cv2.rectangle(frame, (x, y), (x+w, y+h), (255, 0, 0), 5)

            # ridimensiona l'immagine grigia 200x200
            face_img = cv2.resize(gray[y:y+h, x:x+w], (200, 200))

            # riconoscimento occhi

            # salva il frame grigio e ridimensionato nella cartella impostata
            face_filename = '%s/%d.pgm' % (output_folder, count)
            print('face_filename: '+face_filename)
            cv2.imwrite(face_filename, face_img)
            count+=1

        cv2.imshow('Capturing Faces...', frame)
        cv2.imshow('Conversion', gray)

camera.release()
cv2.destroyAllWindows()