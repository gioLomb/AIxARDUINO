import serial
import time
import os
import cv2
import numpy
import datetime
import sys

# Funzione per leggere le immagini di volti registrati
def read_images(path, image_size):
    names = []
    training_images, training_labels = [], []
    label = 0
    for dirname, subdirnames, filenames in os.walk(path):
        for subdirname in subdirnames:
            names.append(subdirname)
            subject_path = os.path.join(dirname, subdirname)
            for filename in os.listdir(subject_path):
                img = cv2.imread(os.path.join(subject_path, filename), cv2.IMREAD_GRAYSCALE)
                if img is None:
                    continue
                img = cv2.resize(img, image_size)
                print('\nloading ', img, '[', label, ']')
                training_images.append(img)
                training_labels.append(label)
            label += 1
    training_images = numpy.asarray(training_images, numpy.uint8)
    training_labels = numpy.asarray(training_labels, numpy.int32)
    return names, training_images, training_labels

# Impostazioni per il riconoscimento facciale
path_to_training_images = './faces-db'
training_image_size = (200, 200)
names, training_images, training_labels = read_images(path_to_training_images, training_image_size)
model = cv2.face.LBPHFaceRecognizer_create()
model.train(training_images, training_labels)
face_cascade = cv2.CascadeClassifier(f'{cv2.data.haarcascades}haarcascade_frontalface_default.xml')

# Inizializzazione della comunicazione seriale con Arduino
arduino = serial.Serial("COM3", 921600, timeout=0.1)

# Funzione per scrivere e leggere da Arduino
# Funzione per scrivere e leggere da Arduino con timeout
def write_read(x):
    arduino.write(bytes(x, 'utf-8'))
    time.sleep(0.05)
    data = arduino.readline().decode().strip()
    return data


# Inizializzazione della videocamera
camera = cv2.VideoCapture(0)
frame_n = 0

# Variabili per il riconoscimento facciale
conti = 0
contFrame = 0
recognized_person = ""

# Attendi fino a quando Arduino invia la stringa "dio"

while True:
   # print("in attesa")
    if arduino.in_waiting > 0:
        line = arduino.readline().decode().strip()
        print(line)
        if line == "now":

                while cv2.waitKey(1) == -1:
                    if contFrame == 30:
                        contFrame=0
                        break
                    success, frame = camera.read()
                    if success:
                        faces = face_cascade.detectMultiScale(frame, 1.3, 5)
                        if len(faces) == 0:
                            contFrame = 0
                        for (x, y, w, h) in faces:
                            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 1)
                            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
                            roi_gray = gray[y:y + h, x:x + w]
                            if roi_gray.size == 0:
                                conti = 0
                                continue
                            roi_gray = cv2.resize(roi_gray, training_image_size)
                            contFrame += 1
                            label, confidence = model.predict(roi_gray)
                            conti += 1
                            conto = str(conti)
                            print(conto)
                            if contFrame == 30:
                                data_attuale = datetime.datetime.now()

                                # Convertire la data in formato stringa
                                data_attuale_str = data_attuale.strftime("%Y-%m-%d %H:%M:%S")

                                # Concatenare la stringa con la data
                                recognized_person = names[label] + " " + data_attuale_str
                                print("Person recognized:", recognized_person)
                                value = write_read(recognized_person)
                                print("ARDUINO1:",value)

                                # Qui puoi fare ciò che vuoi con la persona riconosciuta
                                break
                            text = '%s, confidence=%.2f' % (names[label], confidence)
                            cv2.putText(frame, text, (x, y - 20), cv2.FONT_HERSHEY_SIMPLEX, .5, (255, 85, 0), 2)
                        cv2.imshow('Face Recognition', frame)


# Loop principale



print('\nProgram terminated')
