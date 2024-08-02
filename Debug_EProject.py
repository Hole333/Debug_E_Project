import cv2
import numpy as np
import serial
import time
import math

ser = serial.Serial("/dev/ttyAMA0",115200)
if not ser.isOpen():
    print("open failed")
else:
    print("open success: ")
    print(ser)
cap = cv2.VideoCapture(0)
cap.set(3, 640)
cap.set(4, 640)
worldCenterPoint = [380, 280] # 假设世界坐标系原点在像素坐标系下的映射是50, 50, 实际根据摄像头所看到的实际原点的像素点作为世界坐标系和
                            # 和像素坐标系的映射
total = 9
startFlag = False
outerAreaFlag = False
finishFlag = False
chessArea = []          
outerArea = []
cheqquerList = []
boudingRect = []
newChessDic = {}
##############棋盘所对应的是否有棋子的状态标志位############
WHITE_CHE = 1
BLACK_CHE = 2
NONE = 0
##############棋盘对应区域是否有棋子存在####################
chequerArea = {1 : NONE, 2 : NONE, 3 : NONE,
               4 : NONE, 5 : NONE, 6 : NONE,
               7 : NONE, 8 : NONE, 9 : NONE}
trueWorldR = {1 : [32, 32], 2 : [0, 32], 3 : [-32, 32],
              4 : [32,  0], 5 : [0,  0], 6 : [-32, 0],
              7 : [32, -32], 8 :[0, -32],9 : [-32, -32]}
color = [(255, 0, 0), (0, 255, 255), (255, 255, 0),
         (255, 0, 255), (157, 157, 120), (100, 200, 25),
         (120, 78, 28), (38, 38, 255), (155, 120, 160)]
'''
    寻找落子临近点
'''
def chequerPoint(cheP, coords):
    minLength = 999999999
    minCoordKey = NONE
    for key in coords:
        length = (cheP[0] - coords[key][0]) ** 2 + (cheP[1] - coords[key][1]) ** 2
        if length < minLength:
            minLength = length
            minCoordKey = key
    return minCoordKey

'''
    根据棋盘外矩形框标定九宫格并编码
'''
def encodeAreaPoints(coords, chessCoords, frame):
    encodePoint2 = {}
    crossPoint = []
    corner = sorted(coords, key = lambda x : x[1], reverse=False)
    if corner[0][0] > corner[1][0]:
        corner[0], corner[1] = corner[1], corner[0]
    if corner[2][0] < corner[3][0]:
        corner[2], corner[3] = corner[3], corner[2]
    for i in range(4):
        for j in range(4):
            cross_x = int((corner[0][0] * (3 - i) + corner[1][0] * i) * (3 - j) / 9
                          +(corner[3][0] * (3 - i) + corner[2][0] * i) * j / 9)
            cross_y = int((corner[0][1] * (3 - i) + corner[1][1] * i) * (3 - j) / 9
                          + (corner[3][1] * (3 - i) + corner[2][1] * i) * j / 9)
            crossPoint.append([cross_x, cross_y])
            cv2.circle(frame, (cross_x, cross_y), 3, (0, 255, 0), -1)
    index = 1
    for i in range(3):
        for j in range(3):
            center_x = int((crossPoint[i + j * 4][0] + crossPoint[i + j * 4 + 1][0]
                            + crossPoint[i + (j+1) * 4][0] + crossPoint[i + (j + 1) * 4 + 1][0]) / 4)
            center_y = int((crossPoint[i + j * 4][1] + crossPoint[i + j * 4 + 1][1]
                            + crossPoint[i + (j+1) * 4][1] + crossPoint[i + (j + 1) * 4 + 1][1]) / 4)
            encodePoint2.update({index : [center_x, center_y]})
            cv2.circle(frame, (center_x, center_y), 3, (0, 255, 0), -1)
            index += 1
    return [encodePoint2, corner]

'''
    坐标点映射转换
'''
def img2world(coords, corners):
    worldPoint = {}
    worldLength = 98 # mm
    l1 = math.sqrt((corners[0][0] - corners[1][0]) ** 2 + (corners[0][1] - corners[1][1]) ** 2)
    l2 = math.sqrt((corners[1][0] - corners[2][0]) ** 2 + (corners[1][1] - corners[2][1]) ** 2)
    l3 = math.sqrt((corners[2][0] - corners[3][0]) ** 2 + (corners[2][1] - corners[3][1]) ** 2)
    l4 = math.sqrt((corners[3][0] - corners[0][0]) ** 2 + (corners[3][1] - corners[0][1]) ** 2)
    avgl = (l1 + l2 + l3 + l4) / 4  # 棋盘平均像素长度
    perPixelLength =  worldLength / avgl
    # 坐标轴移动，相对于坐标轴静止， 棋盘往坐标轴反方向运动
    for key in coords:
        worldPoint.update({key : [100 - int((coords[key][0] - worldCenterPoint[0]) * perPixelLength), int((worldCenterPoint[1] - coords[key][1]) * perPixelLength) + 100]})
    return worldPoint


def isInCorner(point, corners, frame):
    vector2_1 = [corners[0][0] - corners[3][0], corners[0][1] - corners[3][1]]
    vector4_1 = [corners[2][0] - corners[3][0], corners[2][1] - corners[3][1]]
    vector4_3 = [corners[2][0] - corners[1][0], corners[2][1] - corners[1][1]]
    vector2_3 = [corners[0][0] - corners[1][0], corners[0][1] - corners[1][1]]
    vectorP_1 = [point[0] - corners[3][0], point[1] - corners[3][1]]
    vectorP_3 = [point[0] - corners[1][0], point[1] - corners[1][1]]
    result = vector2_1[0] * vectorP_1[0] + vector2_1[1] * vectorP_1[1]
    result2 = vector4_1[0] * vectorP_1[0] + vector4_1[1] * vectorP_1[1]
    result3 = vector4_3[0] * vectorP_3[0] + vector4_3[1] * vectorP_3[1]
    result4 = vector2_3[0] * vectorP_3[0] + vector2_3[1] * vectorP_3[1]
    if result >= 0 and result2 >= 0 and result3 >= 0 and result4 >= 0:
        return True
    else:
        return False

def calRotateAngle(corners):
    rotateCoord = {}
    vector1 = [corners[1][0] - corners[2][0], corners[1][1] - corners[2][1]]
    vector2 = [0, 0 - corners[2][1]]
    cos = (vector1[0] * vector2[0] + vector1[1] * vector2[1]) / (math.sqrt((vector1[0] ** 2 + vector1[1] ** 2) * (vector2[0] ** 2 + vector2[1] ** 2)))
    arccos = math.acos(cos)
    # c, s = np.cos(arccos), np.sin(arccos)
    # R = np.array([(c, -s), (s, c)])
    # print(arccos * 180 / math.pi)
    angle = arccos * 180 / math.pi
    if vector1[0] > 0:
        for key in trueWorldR:
            # rotate = np.dot(R, trueWorldR[key])
            rotate = [trueWorldR[key][0] * math.cos(arccos) - trueWorldR[key][1] * math.sin(arccos), 
                    trueWorldR[key][0] * math.sin(arccos) + trueWorldR[key][1] * math.cos(arccos)]
            rotateCoord.update({key : [int(rotate[0]) + 100, int(rotate[1]) + 110]})
    else:
        for key in trueWorldR:
            # rotate = np.dot(R, trueWorldR[key])
            rotate = [-(trueWorldR[key][0] * math.cos(math.pi - arccos) - trueWorldR[key][1] * math.sin(math.pi - arccos)), 
                    -(trueWorldR[key][0] * math.sin(math.pi - arccos) + trueWorldR[key][1] * math.cos(math.pi - arccos))]
            rotateCoord.update({key : [int(rotate[0]) + 100, int(rotate[1]) + 110]})
    return [rotateCoord, int(angle)]
    
def _NOP_ ():
    return

while True:
    ret, frame = cap.read()
    # frame = frame[58 : 477, 116 : 514]
    count = ser.inWaiting()
    if count > 0:
        data = ser.read(count)[0].to_bytes(1, "little")
        if data == b'\xef':
            startFlag = True
    # else:
    #     startFlag = True
    RGBImage = frame
    img = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    ##############对识别黑白棋子做预处理###################
    # hsvImg = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    # lower1 = np.array([0, 43, 46])
    # upper1 = np.array([10, 255, 255])
    # mask1 = cv2.inRange(hsvImg, lower1, upper1)
    # lower2 = np.array([156, 43, 46])
    # upper2 = np.array([180, 255, 255])
    # mask2 = cv2.inRange(hsvImg, lower2, upper2)
    # mask3 = mask1 + mask2 # 棋盘红色区域掩膜
    # mask3 = cv2.bitwise_not(mask3)
    # kernel = np.ones((3, 3), np.uint8)
    # mask3 = cv2.erode(mask3, kernel, iterations = 1)
    # chess = cv2.bitwise_and(RGBImage, RGBImage, mask = mask3)
    ########################黑色棋子掩膜#########################
    # ret, maskBlack = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)
    # #maskBlack = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 11, 2)
    # # blackLower = np.array([0, 0, 0])
    # # blackUpper = np.array([180, 255, 60])
    # # maskBlack = cv2.inRange(hsvImg, blackLower, blackUpper)
    # BlackChequers = cv2.bitwise_and(maskBlack, mask3)
    # kernel = np.ones((3, 3), np.uint8)
    # BlackChequers = cv2.dilate(BlackChequers, kernel, iterations = 1)
    # kernel = np.ones((3, 3), np.uint8)
    # BlackChequers = cv2.erode(BlackChequers, kernel, iterations = 1)
    # BlackChequers = cv2.medianBlur(BlackChequers, 5)
    ######################白色棋子掩膜#############################
    img = cv2.GaussianBlur(img, (3, 3), 0)
    ret, maskWhite = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    kernel = np.ones((5, 5), np.uint8)
    maskWhite = cv2.dilate(maskWhite, kernel, iterations = 1)
    kernel = np.ones((5, 5), np.uint8)
    maskWhite = cv2.erode(maskWhite, kernel, iterations = 1)

    # kernel = np.ones((3, 3), np.uint8)
    # WhiteChequers = cv2.erode(WhiteChequers, kernel, iterations = 1)
    # kernel = np.ones((2, 2), np.uint8)
    # WhiteChequers = cv2.dilate(WhiteChequers, kernel, iterations = 1)
    # WhiteChequers = cv2.medianBlur(WhiteChequers, 5)
    ######################霍夫变换######################
    try:
        midImage = cv2.medianBlur(img, 5)
        circles = cv2.HoughCircles(midImage, method = cv2.HOUGH_GRADIENT, dp = 1, minDist = 40,
                                    param2 = 25, minRadius = 10, maxRadius= 30)
        circle = np.uint16(np.around(circles))
        _, img_the = cv2.threshold(midImage, 80, 255, cv2.THRESH_BINARY)
        for c in circle[0, :]:
            cheqquerList.append([c[0], c[1], c[2]])
    except:
        _NOP_()
    ##############棋盘部分图像预处理#######################
    img = cv2.GaussianBlur(img, (3, 3,), 0)
    binaryImage = cv2.Canny(img, 50, 170)
    kernel = np.ones((5, 5), np.uint8)
    # binaryImage = cv2.erode(binaryImage, kernel, iterations = 1)
    binaryImage = cv2.dilate(binaryImage, kernel, iterations = 1)
    counters, hierarchy = cv2.findContours(binaryImage, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    index = 0
    #####################寻找棋盘外接轮廓##################
    for obj in counters:
        area = cv2.contourArea(obj)
        perimeter = cv2.arcLength(obj, True)
        approx = cv2.approxPolyDP(obj, 0.02 * perimeter, True)
        CornerNum = len(approx)
        x, y, w, h = cv2.boundingRect(approx)
        # 只要满足找到最大正方形条件就可以获得九宫格分割图像
        if area > 15000 and area < 90000 and CornerNum == 4 and (w / h) < 1.3 and (w / h) > 0.8 and outerAreaFlag == False:
            # print(area, (w / h))
            outerAreaFlag = True
            boudingRect = [x, y, w, h]
            ###########可视化四个外接棋盘四个角点###########
            for p in approx:
                cv2.drawMarker(frame, (p[0][0], p[0][1]), (255, 0, 255))
                outerArea.append([p[0][0], p[0][1]])
        if outerAreaFlag == True:
            finishFlag = True
            break
    # 有找到棋盘最大外接正方形
    if outerArea and finishFlag:
        #print(list(outerArea))
        newChessDic, outerArea = encodeAreaPoints(outerArea, chessArea, frame)
        #print(outerArea)
        for key in newChessDic:
            cv2.putText(frame, str(key), (int(newChessDic[key][0]) - 5, int(newChessDic[key][1]) + 8),cv2.FONT_HERSHEY_SIMPLEX,1 ,color[key - 1], 2)
        for c in cheqquerList:
            if isInCorner((c[0], c[1]), outerArea, frame):
                cv2.circle(frame, (c[0], c[1]), c[2], (0, 255, 0))
                currKey = chequerPoint((int(c[0]), int(c[1])), newChessDic)
                if maskWhite[c[1]][c[0]] == 255:
                    chequerArea[currKey] = WHITE_CHE
                else:
                    chequerArea[currKey] = BLACK_CHE
        # counters, hierarchy = cv2.findContours(BlackChequers, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        # counters2, hierarchy2 = cv2.findContours(WhiteChequers, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        # ####################黑色棋子是否落在棋盘上#########################
        # for obj in counters:
        #     area = cv2.contourArea(obj) # 棋子的面积
        #     (x, y), r = cv2.minEnclosingCircle(obj)
        #     if area > 700 and area < 3000 and x > boudingRect[0] and x < (boudingRect[0] + boudingRect[2]) \
        #         and y > boudingRect[1] and y < (boudingRect[1] + boudingRect[3]):
        #         # print(area)
        #         currKey = chequerPoint((int(x), int(y)), newChessDic)
        #         chequerArea[currKey] = BLACK_CHE
        #         cv2.circle(frame, (int(x), int(y)), int(r), (255, 0, 0), 2)
        # ###################白色棋子是否落在棋盘上##################################
        # for obj in counters2:
        #     area = cv2.contourArea(obj) # 棋子的面积
        #     (x, y), r = cv2.minEnclosingCircle(obj)
        #     if area > 700 and area < 3000 and x > boudingRect[0] and x < (boudingRect[0] + boudingRect[2]) \
        #         and y > boudingRect[1] and y < (boudingRect[1] + boudingRect[3]):
        #         # print(area)
        #         currKey = chequerPoint((int(x), int(y)), newChessDic)
        #         # print(currKey)
        #         chequerArea[currKey] = WHITE_CHE
        #         cv2.circle(frame, (int(x), int(y)), int(r), (0, 0, 235), 2)
        # print(chequerArea)
        ##################对棋子像素坐标转化为世界坐标#############################
        worldChessDic = img2world(newChessDic, outerArea)
        rotationDic, roatateAngle = calRotateAngle(outerArea)
        # print(rotationDic)
        # for key in worldChessDic:
        #     print("key", str(key), "dx:", (worldChessDic[key][0] - rotationDic[key][0]), "dy:", 
        #           (worldChessDic[key][1] - rotationDic[key][1]))
        #print(worldChessDic)     
        ################# 串口通讯发送棋牌信息给主控##############################
        #发送格式： 编号 + x坐标 + y坐标 + 角度 + 当前编号是否棋子存在 + 两字节帧尾校验位0xfb 0xae
        if startFlag == True:
   
            for key in rotationDic:
                #print(chequerArea[key])
                ser.write(key.to_bytes(1, "little"))
                ser.write(rotationDic[key][0].to_bytes(2, "little", signed = True))
                ser.write(rotationDic[key][1].to_bytes(2, "little", signed = True))
                time.sleep(0.005)  # 防止一次性发送太多字节数据丢失
                ser.write(roatateAngle.to_bytes(1, "little"))
                ser.write(chequerArea[key].to_bytes(1, "little"))
                time.sleep(0.005)  # 防止一次性发送太多字节数据丢失
                ser.write(b'\xfb')
                ser.write(b'\xae') 
                print(key.to_bytes(1, "little"), rotationDic[key][0].to_bytes(2, "little", signed = True), rotationDic[key][1].to_bytes(2, "little", signed = True) \
                ,roatateAngle.to_bytes(1, "little"), chequerArea[key].to_bytes(1, "little"), b'\xfb', b'\xae')
            ################终止校验位############################
            time.sleep(0.005) # 防止一次性发送太多字节数据丢失
            ser.write(b'\xec')
            ser.write(b'\xbf')
        ##################对落在棋盘上棋子可视化###################################
        for key in chequerArea:
            if chequerArea[key] == BLACK_CHE:
                cv2.circle(frame, (newChessDic[key][0], newChessDic[key][1]), 20, (0, 0, 0), -1)
            elif chequerArea[key] == WHITE_CHE:
                cv2.circle(frame, (newChessDic[key][0], newChessDic[key][1]), 20, (255, 255, 255), -1)
            else:
                continue
    #参考世界坐标系可视化#
    cv2.line(frame, (worldCenterPoint[0], 0), (worldCenterPoint[0], 640), (255, 0, 255))
    cv2.line(frame, (0, worldCenterPoint[1]), (640, worldCenterPoint[1]), (255, 0, 255))
    chessArea.clear()
    outerArea.clear()
    newChessDic.clear()
    cheqquerList.clear()
    chequerArea = {1 : NONE, 2 : NONE, 3 : NONE,
                    4 : NONE, 5 : NONE, 6 : NONE,
                    7 : NONE, 8 : NONE, 9 : NONE}
    finishFlag = False
    outerAreaFlag = False
    startFlag = False
    cv2.imshow("binary", binaryImage)
    # cv2.imshow("gray", img)
    # cv2.imshow("red", mask3)
    # cv2.imshow("black", BlackChequers)
    cv2.imshow("white", maskWhite)
    cv2.imshow("video", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
    
    
    
    

