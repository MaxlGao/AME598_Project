import logging
import requests
import base64
import cv2
import numpy as np
import os

# Setup basic logging
logging.basicConfig(level=logging.INFO)

IS_CV_4 = cv2.__version__[0] == '4'
__version__ = "1.0"  # Adaptive seed


class Object_detect():

    def __init__(self, camera_x = 150, camera_y = 10):
        # inherit the parent class
        super(Object_detect, self).__init__()

        # yolov5 model file path
        self.path = os.path.dirname(os.path.abspath(__file__))
        self.modelWeights = self.path + "/yolov5s.onnx"
        if IS_CV_4:
            self.net = cv2.dnn.readNet(self.modelWeights)
        else:
            print('Load yolov5 model need the version of opencv is 4.')
            exit(0)
            
        # Constants.
        self.INPUT_WIDTH = 640   # default 640
        self.INPUT_HEIGHT = 640  # default 640
        # default 0.5, 0.45, 0.45
        self.SCORE_THRESHOLD = 0.45
        self.NMS_THRESHOLD = 0.40 
        self.CONFIDENCE_THRESHOLD = 0.40
        
        # Text parameters.
        self.FONT_FACE = cv2.FONT_HERSHEY_SIMPLEX
        self.FONT_SCALE = 0.7
        self.THICKNESS = 1
        
        # Colors.
        self.BLACK  = (0,0,0)
        self.BLUE   = (255,178,50)
        self.YELLOW = (0,255,255)
        
        classesFile = self.path + "/coco.names"
        self.classes = None
        with open(classesFile, 'rt') as f:
            self.classes = f.read().rstrip('\n').split('\n')

    def transform_frame(self, frame):
        # enlarge the image by 1.5 times
        fx = 1.5
        fy = 1.5
        frame = cv2.resize(frame, (0, 0), fx=fx, fy=fy,
                           interpolation=cv2.INTER_CUBIC)
        return frame

    def draw_label(self,img,label,x,y):
        text_size = cv2.getTextSize(label,self.FONT_FACE,self.FONT_SCALE,self.THICKNESS)
        dim,baseline = text_size[0],text_size[1]
        cv2.rectangle(img,(x,y),(x+dim[0],y+dim[1]+baseline),(0,0,0),cv2.FILLED)
        cv2.putText(img,label,(x,y+dim[1]),self.FONT_FACE,self.FONT_SCALE,self.YELLOW,self.THICKNESS)

    def pre_process(self,input_image,net):
        blob = cv2.dnn.blobFromImage(input_image,1/255,(self.INPUT_HEIGHT,self.INPUT_WIDTH),[0,0,0], 1, crop=False)
        # Sets the input to the network.
        net.setInput(blob)
        # Run the forward pass to get output of the output layers.
        outputs = net.forward(net.getUnconnectedOutLayersNames())
        return outputs
    
    # detect object
    def post_process(self,input_image):
        class_ids = []
        confidences = []
        boxes = []
        blob = cv2.dnn.blobFromImage(input_image,1/255,(self.INPUT_HEIGHT,self.INPUT_WIDTH),[0,0,0], 1, crop=False)
        # Sets the input to the network.
        self.net.setInput(blob)
        # Run the forward pass to get output of the output layers.
        outputs = self.net.forward(self.net.getUnconnectedOutLayersNames())
        
        rows = outputs[0].shape[1]
        image_height ,image_width = input_image.shape[:2]
        
        x_factor = image_width/self.INPUT_WIDTH
        y_factor = image_height/self.INPUT_HEIGHT
        cx = 0 
        cy = 0 
        largest_box_area = 0
        largest_box_index = -1
        try:
            for r in range(rows):
                row = outputs[0][0][r]
                confidence = row[4]
                if confidence>self.CONFIDENCE_THRESHOLD:
                    classes_scores = row[5:]
                    class_id = np.argmax(classes_scores)


                    # # Check if the detected class is "person" (usually class_id 0 for COCO dataset)
                    # if class_id != 0:  # Replace 0 with the correct class_id for "person" if it's different
                    #     continue  # Skip detections that are not "person"


                    if (classes_scores[class_id]>self.SCORE_THRESHOLD):
                        confidences.append(confidence)
                        class_ids.append(class_id)
                        cx,cy,w,h = row[0],row[1],row[2],row[3]
                        left = int((cx-w/2)*x_factor)
                        top = int((cy - h/2) * y_factor)
                        width = int(w * x_factor)
                        height = int(h * y_factor)
                        box = np.array([left, top, width, height])
                        boxes.append(box)
                        
                        
                        indices = cv2.dnn.NMSBoxes(boxes, confidences, self.CONFIDENCE_THRESHOLD, self.NMS_THRESHOLD)
                       
                        for i in indices:
                            box = boxes[i]
                            left = box[0]
                            top = box[1]
                            width = box[2]
                            height = box[3]
                                    
                            cv2.rectangle(input_image, (left, top), (left + width, top + height),self.BLUE, 3*self.THICKNESS)
                           
                            cx = left+(width)//2 
                            cy = top +(height)//2
                           
                            cv2.circle(input_image, (cx,cy),  5,self.BLUE, 10)

                            label = "{}:{:.2f}".format(self.classes[class_ids[i]], confidences[i])             
                             
                            self.draw_label(input_image, label, left, top)
                            
        except Exception as e:
            print(e)
            exit(0)

        if cx + cy > 0:
            return cx, cy, input_image
        else:
            return None


status = True
def camera_status():
    global status
    status = True
    cap = cv2.VideoCapture(0)
    
def send_data_to_server(x, y, image=None):
    server_url = 'http://34.207.112.40:8080/setValue'
    data = {'pitch': y, 'yaw': x}

    if image is not None:
        # Downscale the image to reduce size
        scale_percent = 50  # percent of original size, change as needed
        width = int(image.shape[1] * scale_percent / 100)
        height = int(image.shape[0] * scale_percent / 100)
        dim = (width, height)
        resized_image = cv2.resize(image, dim, interpolation=cv2.INTER_AREA)

        # Adjust JPEG quality for further compression
        jpeg_quality = 70  # Adjust quality as needed (0-100, lower means more compression)
        _, encoded_image = cv2.imencode('.jpg', resized_image, [int(cv2.IMWRITE_JPEG_QUALITY), jpeg_quality])
        image_base64 = base64.b64encode(encoded_image).decode('utf-8')
        data['image'] = image_base64

    try:
        response = requests.post(server_url, json=data)
        print("Data sent successfully. Server response:", response.text)
    except requests.exceptions.RequestException as e:
        print("Failed to send data to server:", e)

def runs():
    global status
    detect = Object_detect()

    cap = cv2.VideoCapture(0)
        

    try:
        while True:
            if not status:
                cap = cv2.VideoCapture(0)
                status = True
            ret, frame = cap.read()

            detect_result = detect.post_process(frame)
            if detect_result:
                x, y, input_img = detect_result
                image_height, image_width = input_img.shape[:2]
                x = int(x)
                y = int(y)
                cenx = int(image_width / 2)
                ceny = int(image_height / 2)

                # Centering algorithm
                yaw_command = "right" if x - cenx > 0 else "left"
                pitch_command = "down" if y - ceny > 0 else "up"

                logging.info(f"Yaw: {yaw_command}, Pitch: {pitch_command}")
                send_data_to_server(yaw_command, pitch_command, input_img)
            else:
                logging.info("Failed to detect")

    except KeyboardInterrupt:
        logging.info("Stopping the script")
        cap.release()
                

if __name__ == "__main__":

    runs()
    

