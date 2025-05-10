<div align="center">
   <h1>C++_FaceLiveness_AI_WebServer (Windows, Linux, Docker)</h1>
</div>



### **1. Core Functionality**

This code implements a **Face Liveness Detection HTTP Service** with three key capabilities:

1. **Anti-Spoofing Detection** (Genuine vs Fake)
2. **Face Quality Assessment**
3. **Real-time License Validation**

------

### **2. System Architecture**

#### **2.1 Component Diagram**

```
plaintextCopyHTTP Server (Poco C++)
├── License Manager
├── Face SDK Integration
├── Image Processing Pipeline
└── API Endpoints
```

#### **2.2 Key Technical Parameters**

| Parameter               | Value/Range | Description                    |
| ----------------------- | ----------- | ------------------------------ |
| Liveness Threshold      | 0.5         | Decision boundary for spoofing |
| Quality Threshold       | 0.5         | Minimum acceptable quality     |
| License Check Interval  | 10 seconds  | Real-time validation           |
| Max Processing Attempts | 2           | Error recovery mechanism       |

------

### **3. Core Workflow**

#### **3.1 Processing Sequence**

```
1. License Validation → 
2. Image Acquisition (Multipart/Base64) → 
3. Temporary File Storage → 
4. Face SDK Initialization → 
5. Liveness Detection → 
6. Quality Assessment → 
7. JSON Response Generation
```

#### **3.2 Dynamic SDK Loading**

```
cppCopypipeline_check_liveness = GetProcAddress(...)  // Load SDK functions
image_create_path = GetProcAddress(...)        // Platform-specific image handling
```

------

### **4. Key Technical Features**

#### **4.1 Anti-Spoofing Logic**

```
if(result.quality_result.score < 0.5) → "Bad Quality"
elif(liveness_result.probability >= 0.5) → "Genuine"
else → "Spoofed"
```

#### **4.2 Image Handling**

- Multi-Format Support

  :

  - Multipart form uploads
  - Base64 encoded payloads

- Temporary File Strategy

  :

  ```
  std::string filePath = millisecondsStr + "output_file.dat"; // Unique filename
  ```

#### **4.3 Error Recovery**

```
for (int i = 0; i < 2; i++) { // Dual attempt mechanism
    if(license_error) {
        pipeline_destroy(g_pPipeline);
        setting_init(1); // Reset pipeline
    }
}
```

------

### **5. Security Implementation**

#### **5.1 License Management**

- CRITICAL_SECTION Protection

  :

  ```
  InitializeCriticalSection(&g_cs); // Thread-safe license access
  ```

- Validation Logic

  :

  ```
  #ifdef NDEBUG // Strict checks in release builds
  if(lv_pstRes->m_lExpire < now_c) → Block processing
  #endif
  ```

#### **5.2 Data Sanitization**

- Base64 cleanup:

  ```
  encodedImage = replaceAll(encodedImage, "\r\n", "");
  ```

------

### **6. Performance Considerations**

#### **6.1 Timing Mechanism**

```
uint64_t getMilliseconds() { // Cross-platform implementation
    #ifdef WIN64
        return GetTickCount64();
    #else
        clock_gettime(...)
    #endif
}
```

#### **6.2 Resource Management**

- Automatic Cleanup

  :

  ```
  std::remove(filePath.c_str()); // Delete temp files
  image_destroy(image); // SDK resource release
  ```

------

### **7. API Response Structure**

#### **7.1 JSON Output Format**

```
{
    "score": 0.85,
    "probability": 0.92,
    "quality": 0.78,
    "liveness result": "Image is genuine",
    "state": "OK"
}
```

#### **7.2 Error Handling**

| HTTP Status  | Scenario                |
| ------------ | ----------------------- |
| 200 OK       | Valid results           |
| 409 CONFLICT | Processing errors       |
| 200 (Custom) | License expired/missing |

------

### **8. Results**

<div align="center">
   <img src=https://github.com/LucaIT523/C_FaceLiveness_AI_WebServer/blob/main/images/1.png>
</div>