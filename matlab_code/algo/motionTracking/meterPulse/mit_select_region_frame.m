function [roi_masks, full_masks] = mit_select_region_frame(rgbframe, params, forced_selection, cv_package)
    MASKIN_CORNER_LOWER = params{1}{1};
    MASKIN_CORNER_UPPER = params{1}{2};
    
    if length(params) == 2
        MASKOUT_CORNER_LOWER = params{2}{1};
        MASKOUT_CORNER_UPPER = params{2}{2};
    end

    switch cv_package
        case 'opencv'
            % Load cascade files
            xml_file = 'haarcascade_frontalface_alt2.xml';
            face_classifier = cv.CascadeClassifier(which(xml_file));

            % Perform detection
            greyframe = cv.cvtColor(rgbframe,'RGB2GRAY');
            greyframe = cv.equalizeHist(greyframe);

            % Detect
            if ~forced_selection
                face_boxes = face_classifier.detect(greyframe,'ScaleFactor',1.3,...
                                             'MinNeighbors',3,...
                                             'MinSize',[30,30]);
            else
                face_boxes = {[1, 1, size(greyframe, 2) - 1, size(greyframe, 1) - 1]};    
            end

        case 'native'
    end

    if length(params) == 2
        roi_masks = cell(numel(face_boxes), 2);
    else
        roi_masks = cell(numel(face_boxes), 1);
    end
    full_masks = cell(numel(face_boxes), 1);

    for i = 1 : numel(face_boxes)
        selection_mask = face_boxes{i};

        selection_mask(1) = ceil(selection_mask(1) + selection_mask(3) * MASKIN_CORNER_LOWER(1));
        selection_mask(2) = ceil(selection_mask(2) + selection_mask(4) * MASKIN_CORNER_LOWER(2));
        selection_mask(3) = floor(selection_mask(3) * (MASKIN_CORNER_UPPER(1) - MASKIN_CORNER_LOWER(1)));
        selection_mask(4) = floor(selection_mask(4) * (MASKIN_CORNER_UPPER(2) - MASKIN_CORNER_LOWER(2)));
        roi_masks{i, 1} = selection_mask;

        if length(params) == 2
            maskout(1) = ceil(selection_mask(1) + selection_mask(3) * MASKOUT_CORNER_LOWER(1));
            maskout(2) = ceil(selection_mask(2) + selection_mask(4) * MASKOUT_CORNER_LOWER(2));
            maskout(3) = floor(selection_mask(3) * (MASKOUT_CORNER_UPPER(1) - MASKOUT_CORNER_LOWER(1)));
            maskout(4) = floor(selection_mask(4) * (MASKOUT_CORNER_UPPER(2) - MASKOUT_CORNER_LOWER(2)));
            roi_masks{i, 2} = maskout;
        end


        mask = false(size(greyframe));
        mask(roi_masks{i, 1}(2) : roi_masks{i, 1}(2) + roi_masks{i, 1}(4), ...
            roi_masks{i, 1}(1) : roi_masks{i, 1}(1) + roi_masks{i, 1}(3)) = true;
        
        if length(params) == 2
            mask(roi_masks{i, 2}(2) : roi_masks{i, 2}(2) + roi_masks{i, 2}(4), ...
                roi_masks{i, 2}(1) : roi_masks{i, 2}(1) + roi_masks{i, 2}(3)) = false;
        end
        
        full_masks{i} = mask;
    end
end