//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using FukumaroFaceRecognition;
using SDKTemplate.Helpers;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Media.Capture;
using Windows.Media.FaceAnalysis;
using Windows.Media.MediaProperties;
using Windows.Storage;
//using OpenCVBridge;
using Windows.Storage.Streams;
using Windows.System.Threading;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Shapes;
using System.Linq;

using Windows.ApplicationModel;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.Graphics.Display;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Media.Core;
using Windows.Media.Capture;
using Windows.Media.MediaProperties;
using Windows.Phone.UI.Input;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Streams;
using Windows.System.Display;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Shapes;
using Windows.Media.FaceAnalysis;
using Windows.UI;
using System.Collections.Generic;

namespace SDKTemplate
{

  /// <summary>
  /// Page for demonstrating FaceTracking.
  /// </summary>
  public sealed partial class FaceRegisterPace : Page
  {
    //private LoggerUtils logger = new LoggerUtils("TrackFacesInWebcam1");

    //private TrackingHelper _helper;
    public static FukumaroFaceRecognition.RegisterHelper _registerHelper;

    //public  IdenfityHelper _idenfityHelper;

    /// <summary>
    /// Brush for drawing the bounding box around each identified face.
    /// </summary>
    private readonly SolidColorBrush lineBrush = new SolidColorBrush(Windows.UI.Colors.Yellow);

    /// <summary>
    /// Thickness of the face bounding box lines.
    /// </summary>
    private readonly double lineThickness = 2.0;

    /// <summary>
    /// Transparent fill for the bounding box.
    /// </summary>
    private readonly SolidColorBrush fillBrush = new SolidColorBrush(Windows.UI.Colors.Transparent);

    /// <summary>
    /// Reference back to the "root" page of the app.
    /// </summary>
    private MainPage rootPage;

    /// <summary>
    /// Holds the current scenario state value.
    /// </summary>
    //private ScenarioState currentState;

    /// <summary>
    /// References a MediaCapture instance; is null when not in Streaming state.
    /// </summary>
    private MediaCapture mediaCapture;

    /// <summary>
    /// Cache of properties from the current MediaCapture device which is used for capturing the preview frame.
    /// </summary>
    private VideoEncodingProperties videoProperties;


    /// <summary>
    /// References a FaceTracker instance.
    /// </summary>
    private FaceTracker faceTracker;

    /// <summary>
    /// A periodic timer to execute FaceTracker on preview frames
    /// </summary>
    private ThreadPoolTimer frameProcessingTimer;

    /// <summary>
    /// Semaphore to ensure FaceTracking logic only executes one at a time
    /// </summary>
    private SemaphoreSlim frameProcessingSemaphore = new SemaphoreSlim(1);

    private string dbf = Windows.Storage.ApplicationData.Current.LocalFolder.Path + "\\faces.db";
    private string df = Windows.Storage.ApplicationData.Current.LocalFolder.Path + "\\df.dat";
    private string spFile = "models\\dlib\\shape_predictor_with_glass.dat";
    private string genderDf = "models\\dlib\\genderdf.dat";
    private string genderNet = "models\\caffe\\gender\\gender_net.caffemodel";
    private string genderProto = "models\\caffe\\gender\\deploy_gender.prototxt";
    private string ageNet = "models\\caffe\\age\\age_net.caffemodel";
    private string ageProto = "models\\caffe\\age\\deploy_age.prototxt";
    private string imgNet = "models\\dlib\\dlib_face_recognition_resnet_model_v1.dat";
    private string emotionModelFile = "models/caffe/VGG_S_rgb/deploy.prototxt";
    private string emotionWeightsFile = "models/caffe/VGG_S_rgb/EmotiW_VGG_S.caffemodel";


    /// <summary>
    /// Initializes a new instance of the <see cref="FaceRegisterPace"/> class.
    /// </summary>
    public FaceRegisterPace()
    {
      this.InitializeComponent();
      //this.currentState = ScenarioState.Idle;
      //App.Current.Suspending += this.OnSuspending;
      //_previewRenderer = new FrameRenderer(PreviewImage);

      ConfigOption configOption = new ConfigOption
      {
        DBFile = dbf,
        SVMFaceModel = df,
        ShapeModel = spFile,
        ImageNetModel = imgNet,
        GenderSVMModel = genderDf,
        GenderNetModel = genderNet,
        GenderProto = genderProto,
        AgeModel = ageNet,
        AgeProto = ageProto,
        EmotionModelFile = emotionModelFile,
        EmotionWeightsFile = emotionWeightsFile
      };

      IList<ErrorInfo> error = new List<ErrorInfo>();
      _registerHelper = new RegisterHelper(configOption, error);
      if (error.Count > 0)
      {
        Debug.WriteLine("error");
      }
      else
      {
        Debug.WriteLine("Initialize success!!");
        FaceInfo userInfor = new FaceInfo();
        _registerHelper.TrainingStatus(userInfor, error);
      }


      // Do not cache the state of the UI when suspending/navigating
      NavigationCacheMode = NavigationCacheMode.Disabled;

      // Useful to know when to initialize/clean up the camera
      Application.Current.Suspending += Application_Suspending;
      Application.Current.Resuming += Application_Resuming;
    }

    // Receive notifications about rotation of the device and UI and apply any necessary rotation to the preview stream and UI controls
    private readonly DisplayInformation _displayInformation = DisplayInformation.GetForCurrentView();
    private readonly SimpleOrientationSensor _orientationSensor = SimpleOrientationSensor.GetDefault();
    private SimpleOrientation _deviceOrientation = SimpleOrientation.NotRotated;
    private DisplayOrientations _displayOrientation = DisplayOrientations.Portrait;

    // Folder in which the captures will be stored (initialized in SetupUiAsync)
    private StorageFolder _captureFolder = null;

    // Prevent the screen from sleeping while the camera is running
    private readonly DisplayRequest _displayRequest = new DisplayRequest();

    // MediaCapture and its state variables
    private MediaCapture _mediaCapture;
    private IMediaEncodingProperties _previewProperties;
    private bool _isInitialized;
    private bool _isRecording;

    // Information about the camera device
    private bool _mirroringPreview;


    #region Constructor, lifecycle and navigation

    private async void Application_Suspending(object sender, SuspendingEventArgs e)
    {
      // Handle global application events only if this page is active
      if (Frame.CurrentSourcePageType == typeof(MainPage))
      {
        var deferral = e.SuspendingOperation.GetDeferral();

        await CleanupCameraAsync();

        //await CleanupUiAsync();

        deferral.Complete();
      }
    }

    private async void Application_Resuming(object sender, object o)
    {
      // Handle global application events only if this page is active
      if (Frame.CurrentSourcePageType == typeof(MainPage))
      {
        await SetupUiAsync();

        await InitializeCameraAsync();
      }
    }

    protected override async void OnNavigatedTo(NavigationEventArgs e)
    {
      await SetupUiAsync();

      await InitializeCameraAsync();

      if (this.faceTracker == null)
      {
        this.faceTracker = await FaceTracker.CreateAsync();
      }
    }

    protected override async void OnNavigatingFrom(NavigatingCancelEventArgs e)
    {
      // Handling of this event is included for completenes, as it will only fire when navigating between pages and this sample only includes one page

      await CleanupCameraAsync();

      //await CleanupUiAsync();
    }

    #endregion Constructor, lifecycle and navigation


    #region Event handlers

    List<FaceData> faceDatas = new List<FaceData>();
    //int imageCount = 0;
    //private async Task<bool> IdenfityTask(IList<Windows.Media.FaceAnalysis.DetectedFace> faces, SoftwareBitmap frame)
    //{
    //  bool ret = true;
    //  try
    //  {
    //    SoftwareBitmap originalBitmap = SoftwareBitmap.Convert(frame, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
    //    if (faces.Count == 1)
    //    {
    //      FaceData faceData = new FaceData
    //      {
    //        FrameImage = SoftwareBitmap.Copy(originalBitmap),
    //        FaceBB = new FukumaroFaceRecognition.FaceBox
    //        {
    //          Left = (int)faces[0].FaceBox.X,
    //          Right = (int)(faces[0].FaceBox.X + faces[0].FaceBox.Width),
    //          Top = (int)faces[0].FaceBox.Y,
    //          Bottom = (int)(faces[0].FaceBox.Y + faces[0].FaceBox.Height)
    //        }
    //      };

    //      //SoftwareBitmap croppedBitmap = await CropBitmap.CorpSoftwareBitmap(originalBitmap, faces[0]);
    //      //SoftwareBitmap imageBitmap = SoftwareBitmap.Convert(croppedBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);

    //      faceDatas.Add(faceData);
    //    }
    //    //using (SoftwareBitmap convertedSource = SoftwareBitmap.Convert(frame, BitmapPixelFormat.Bgra8))
    //    //{
    //    //    var ignored = this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
    //    //    {
    //    //        //_previewRenderer.RenderFrame(originalBitmap);
    //    //    });

    //    //}

    //  }
    //  catch (Exception ex)
    //  {

    //  }
    //  return ret;

    //}

    private async Task<bool> CaptureButton()
    {
      bool ret = true;

      IList<Windows.Media.FaceAnalysis.DetectedFace> faces = null;
      // Create a VideoFrame object specifying the pixel format we want our capture image to be (NV12 bitmap in this case).
      // GetPreviewFrame will convert the native webcam frame into this format.
      const BitmapPixelFormat InputPixelFormat = BitmapPixelFormat.Nv12;
      using (VideoFrame previewFrame = new VideoFrame(InputPixelFormat, (int)this.videoProperties.Width, (int)this.videoProperties.Height))
      {
        await this._mediaCapture.GetPreviewFrameAsync(previewFrame);

        // The returned VideoFrame should be in the supported NV12 format but we need to verify this.
        if (FaceDetector.IsBitmapPixelFormatSupported(previewFrame.SoftwareBitmap.BitmapPixelFormat))
        {
          faces = await this.faceTracker.ProcessNextFrameAsync(previewFrame);
        }
        else
        {
          throw new System.NotSupportedException("PixelFormat '" + InputPixelFormat.ToString() + "' is not supported by FaceDetector");
        }


        //await IdenfityTask(faces, previewFrame.SoftwareBitmap);

        SoftwareBitmap originalBitmap = SoftwareBitmap.Convert(previewFrame.SoftwareBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
        if (faces.Count == 1)
        {
          FaceData faceData = new FaceData
          {
            FrameImage = SoftwareBitmap.Copy(originalBitmap),
            FaceBB = new FukumaroFaceRecognition.FaceBox
            {
              Left = (int)faces[0].FaceBox.X,
              Right = (int)(faces[0].FaceBox.X + faces[0].FaceBox.Width),
              Top = (int)faces[0].FaceBox.Y,
              Bottom = (int)(faces[0].FaceBox.Y + faces[0].FaceBox.Height)
            }
          };

          //SoftwareBitmap croppedBitmap = await CropBitmap.CorpSoftwareBitmap(originalBitmap, faces[0]);
          //SoftwareBitmap imageBitmap = SoftwareBitmap.Convert(croppedBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);

          faceDatas.Add(faceData);
        }

        if(faceDatas.Count >= 10)
        {
          try
          {
            UserData userData = new UserData();
            userData.UserName = "HD";
            userData.FaceDatas = faceDatas;
            FaceInfo userInfor = new FaceInfo();
            IList<ErrorInfo> error = new List<ErrorInfo>();
            _registerHelper.CreateFace(userData, userInfor, error);
            if (error.Count > 0)
            {
              Debug.WriteLine("error");
            }
            else
            {
              Debug.WriteLine("register done - " + userInfor.PersistedName);
            }
          }
          catch (Exception ex)
          {
            string ddd = ex.Message;
            Debug.WriteLine(ddd);
          }
        }
      }
      return ret;
    }

    private async void PhotoButton_Click(object sender, RoutedEventArgs e)
    {

      //await this.CaptureButton();
      int i = 0;
      while (i < 12)
      {
        await Task.Delay(TimeSpan.FromSeconds(5));
        await this.CaptureButton();
        await TakePhotoAsync();
        i++;
      }

      PhotoButton.IsEnabled = false;
    }


    private async void MediaCapture_RecordLimitationExceeded(MediaCapture sender)
    {
      // This is a notification that recording has to stop, and the app is expected to finalize the recording

      //await StopRecordingAsync();

      await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => UpdateCaptureControls());
    }

    private async void MediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
    {
      Debug.WriteLine("MediaCapture_Failed: (0x{0:X}) {1}", errorEventArgs.Code, errorEventArgs.Message);

      await CleanupCameraAsync();

      await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => UpdateCaptureControls());
    }

    #endregion Event handlers


    #region MediaCapture methods

    /// <summary>
    /// Initializes the MediaCapture, registers events, gets camera device information for mirroring and rotating, starts preview and unlocks the UI
    /// </summary>
    /// <returns></returns>
    private async Task InitializeCameraAsync()
    {
      Debug.WriteLine("InitializeCameraAsync");

      if (_mediaCapture == null)
      {
        // Attempt to get the front camera if one is available, but use any camera device if not
        var cameraDevice = await FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.Front);

        if (cameraDevice == null)
        {
          Debug.WriteLine("No camera device found!");
          return;
        }

        // Create MediaCapture and its settings
        _mediaCapture = new MediaCapture();

        // Register for a notification when video recording has reached the maximum time and when something goes wrong
        _mediaCapture.RecordLimitationExceeded += MediaCapture_RecordLimitationExceeded;
        _mediaCapture.Failed += MediaCapture_Failed;

        var settings = new MediaCaptureInitializationSettings
        {
          VideoDeviceId = cameraDevice.Id,
          SharingMode = MediaCaptureSharingMode.ExclusiveControl,
          StreamingCaptureMode = StreamingCaptureMode.Video,
          MemoryPreference = MediaCaptureMemoryPreference.Cpu
        };

        // Initialize MediaCapture
        try
        {
          await _mediaCapture.InitializeAsync(settings);
          _isInitialized = true;
        }
        catch (UnauthorizedAccessException)
        {
          Debug.WriteLine("The app was denied access to the camera");
        }

        // If initialization succeeded, start the preview
        if (_isInitialized)
        {
          await StartPreviewAsync();
          UpdateCaptureControls();
        }
      }
    }

    /// <summary>
    /// Starts the preview and adjusts it for for rotation and mirroring after making a request to keep the screen on
    /// </summary>
    /// <returns></returns>
    private async Task StartPreviewAsync()
    {
      // Prevent the device from sleeping while the preview is running
      _displayRequest.RequestActive();

      // Set the preview source in the UI and mirror it if necessary
      PreviewControl.Source = _mediaCapture;
      PreviewControl.FlowDirection = _mirroringPreview ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;

      // Start the preview
      await _mediaCapture.StartPreviewAsync();
      _previewProperties = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview);

      // Cache the media properties as we'll need them later.
      var deviceController = this._mediaCapture.VideoDeviceController;
      this.videoProperties = deviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview) as VideoEncodingProperties;

      // Initialize the preview to the current orientation
      if (_previewProperties != null)
      {
        _displayOrientation = _displayInformation.CurrentOrientation;

        //await SetPreviewRotationAsync();
      }
    }


    /// <summary>
    /// Stops the preview and deactivates a display request, to allow the screen to go into power saving modes
    /// </summary>
    /// <returns></returns>
    private async Task StopPreviewAsync()
    {
      // Stop the preview
      _previewProperties = null;
      await _mediaCapture.StopPreviewAsync();

      // Use the dispatcher because this method is sometimes called from non-UI threads
      await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
      {
        // Cleanup the UI
        PreviewControl.Source = null;

        // Allow the device screen to sleep now that the preview is stopped
        _displayRequest.RequestRelease();
      });
    }

    /// <summary>
    /// Takes a photo to a StorageFile and adds rotation metadata to it
    /// </summary>
    /// <returns></returns>
    private async Task TakePhotoAsync()
    {
      var stream = new InMemoryRandomAccessStream();

      Debug.WriteLine("Taking photo...");
      await _mediaCapture.CapturePhotoToStreamAsync(ImageEncodingProperties.CreateJpeg(), stream);

      try
      {
        var file = await _captureFolder.CreateFileAsync("SimplePhoto.jpg", CreationCollisionOption.GenerateUniqueName);

        Debug.WriteLine("Photo taken! Saving to " + file.Path);

        var photoOrientation = PhotoOrientation.Normal;

        await ReencodeAndSavePhotoAsync(stream, file, photoOrientation);

        Debug.WriteLine("Photo saved!");
      }
      catch (Exception ex)
      {
        // File I/O errors are reported as exceptions
        Debug.WriteLine("Exception when taking a photo: " + ex.ToString());
      }
    }

    /// <summary>
    /// Cleans up the camera resources (after stopping any video recording and/or preview if necessary) and unregisters from MediaCapture events
    /// </summary>
    /// <returns></returns>
    private async Task CleanupCameraAsync()
    {
      Debug.WriteLine("CleanupCameraAsync");

      if (_isInitialized)
      {
        if (_previewProperties != null)
        {
          // The call to stop the preview is included here for completeness, but can be
          // safely removed if a call to MediaCapture.Dispose() is being made later,
          // as the preview will be automatically stopped at that point
          await StopPreviewAsync();
        }

        _isInitialized = false;
      }

      if (_mediaCapture != null)
      {
        _mediaCapture.RecordLimitationExceeded -= MediaCapture_RecordLimitationExceeded;
        _mediaCapture.Failed -= MediaCapture_Failed;
        _mediaCapture.Dispose();
        _mediaCapture = null;
      }
    }

    #endregion MediaCapture methods


    #region Helper functions

    /// <summary>
    /// Attempts to lock the page orientation, hide the StatusBar (on Phone) and registers event handlers for hardware buttons and orientation sensors
    /// </summary>
    /// <returns></returns>
    private async Task SetupUiAsync()
    {
      // Attempt to lock page to landscape orientation to prevent the CaptureElement from rotating, as this gives a better experience
      DisplayInformation.AutoRotationPreferences = DisplayOrientations.Landscape;

      // Populate orientation variables with the current state
      _displayOrientation = _displayInformation.CurrentOrientation;
      if (_orientationSensor != null)
      {
        _deviceOrientation = _orientationSensor.GetCurrentOrientation();
      }

      var picturesLibrary = await StorageLibrary.GetLibraryAsync(KnownLibraryId.Pictures);
      // Fall back to the local app storage if the Pictures Library is not available
      _captureFolder = picturesLibrary.SaveFolder ?? ApplicationData.Current.LocalFolder;
    }

    /// <summary>
    /// This method will update the icons, enable/disable and show/hide the photo/video buttons depending on the current state of the app and the capabilities of the device
    /// </summary>
    private void UpdateCaptureControls()
    {
      // The buttons should only be enabled if the preview started sucessfully
      PhotoButton.IsEnabled = _previewProperties != null;

      // If the camera doesn't support simultaneosly taking pictures and recording video, disable the photo button on record
      if (_isInitialized && !_mediaCapture.MediaCaptureSettings.ConcurrentRecordAndPhotoSupported)
      {
        PhotoButton.IsEnabled = !_isRecording;

        // Make the button invisible if it's disabled, so it's obvious it cannot be interacted with
        PhotoButton.Opacity = PhotoButton.IsEnabled ? 1 : 0;
      }
    }


    /// <summary>
    /// Attempts to find and return a device mounted on the panel specified, and on failure to find one it will return the first device listed
    /// </summary>
    /// <param name="desiredPanel">The desired panel on which the returned device should be mounted, if available</param>
    /// <returns></returns>
    private static async Task<DeviceInformation> FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel desiredPanel)
    {
      // Get available devices for capturing pictures
      var allVideoDevices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);

      // Get the desired camera by panel
      DeviceInformation desiredDevice = allVideoDevices.FirstOrDefault(x => x.EnclosureLocation != null && x.EnclosureLocation.Panel == desiredPanel);

      // If there is no device mounted on the desired panel, return the first device found
      return desiredDevice ?? allVideoDevices.FirstOrDefault();
    }

    /// <summary>
    /// Applies the given orientation to a photo stream and saves it as a StorageFile
    /// </summary>
    /// <param name="stream">The photo stream</param>
    /// <param name="file">The StorageFile in which the photo stream will be saved</param>
    /// <param name="photoOrientation">The orientation metadata to apply to the photo</param>
    /// <returns></returns>
    private static async Task ReencodeAndSavePhotoAsync(IRandomAccessStream stream, StorageFile file, PhotoOrientation photoOrientation)
    {
      using (var inputStream = stream)
      {
        var decoder = await BitmapDecoder.CreateAsync(inputStream);

        using (var outputStream = await file.OpenAsync(FileAccessMode.ReadWrite))
        {
          var encoder = await BitmapEncoder.CreateForTranscodingAsync(outputStream, decoder);

          var properties = new BitmapPropertySet { { "System.Photo.Orientation", new BitmapTypedValue(photoOrientation, PropertyType.UInt16) } };

          await encoder.BitmapProperties.SetPropertiesAsync(properties);
          await encoder.FlushAsync();
        }
      }
    }

    #endregion Helper functions

  }

}