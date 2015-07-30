function forehead_boxes = detect_foreheads_frame(rgbframe)
    frame_res = size(rgbframe);
    frame_res = frame_res(1:2);

    if min(frame_res) < 50
        error('Resolution is too low');
    end

    % Load cascade files
    xml_file = 'haarcascade_frontalface_alt2.xml';
    face_classifier = cv.CascadeClassifier(which(xml_file));

    xml_file = 'haarcascade_eye_tree_eyeglasses.xml';
    eye_classifier = cv.CascadeClassifier(which(xml_file));

    % Perform detection
    % im = cv.resize(im,0.5);
    greyframe = cv.cvtColor(rgbframe,'RGB2GRAY');
    greyframe = cv.equalizeHist(greyframe);

    % Detect
    face_boxes = face_classifier.detect(greyframe,'ScaleFactor',1.3,...
                                 'MinNeighbors',3,...
                                 'MinSize',[30,30]);

    forehead_boxes = face_boxes;

    % Draw results
    imshow(rgbframe);
    for i = 1:numel(face_boxes)
        rectangle('Position', face_boxes{i},'EdgeColor','g','LineWidth',2);

        face_box = imcrop(greyframe, face_boxes{i});

        eye_boxes = eye_classifier.detect(face_box,'ScaleFactor',1.3,...
                                 'MinNeighbors',3,...
                                 'MinSize',[5,5]);

        if numel(eye_boxes) == 0
            display('No eyes detected');
            continue
        end

        forehead_box = [inf, 0, 0, inf];
        for j = 1:numel(eye_boxes)
            eye_box = eye_boxes{j} + face_boxes{i} .* [1, 1, 0, 0];
            rectangle('Position', eye_box,'EdgeColor','r','LineWidth',2);
            
            forehead_box(1) = min([eye_boxes{j}(1), forehead_box(1)]);
            forehead_box(3) = max([eye_boxes{j}(1) + eye_boxes{j}(3), forehead_box(3)]);
            forehead_box(4) = min([eye_boxes{j}(2), forehead_box(4)]);
        end
        forehead_box(3) = forehead_box(3) - forehead_box(1);
        forehead_box(4) = forehead_box(4) - forehead_box(2);
        forehead_box = forehead_box + face_boxes{i} .* [1, 1, 0, 0];
        rectangle('Position', forehead_box,'EdgeColor','b','LineWidth',2);

        forehead_boxes{i} = forehead_box;
    end
end