package org.artoolkitx.utilities.cameracalibration;

import android.content.Context;
import android.os.Bundle;
import android.os.CancellationSignal;
import android.os.ParcelFileDescriptor;
import android.print.PageRange;
import android.print.PrintAttributes;
import android.print.PrintDocumentAdapter;
import android.print.PrintDocumentInfo;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Paths;

public class PdfDocumentAdapter extends PrintDocumentAdapter {

    static final String LOGTAG = "PdfDocumentAdapter";
    Context context = null;
    String pathName = null;
    String name = null;
    int resID = 0;
    int pageCount = 0;

    public PdfDocumentAdapter(Context ctxt, String pathName, String name, int pageCount) {
        context = ctxt;
        this.pathName = pathName;
        this.name = name;
        this.resID = 0;
        this.pageCount = pageCount;
    }

    // Use e.g. new PdfDocumentAdapter(getApplicationContext, R.raw.myfile.pdf, 0);
    public PdfDocumentAdapter(Context ctxt, int resID, String name, int pageCount) {
        context = ctxt;
        this.pathName = null;
        this.name = name;
        this.resID = resID;
        this.pageCount = pageCount;
    }

    @Override
    public void onLayout(PrintAttributes printAttributes, PrintAttributes printAttributes1, CancellationSignal cancellationSignal, LayoutResultCallback layoutResultCallback, Bundle bundle) {
        if (cancellationSignal.isCanceled()) {
            layoutResultCallback.onLayoutCancelled();
        } else {
            PrintDocumentInfo.Builder builder = new PrintDocumentInfo.Builder(name);
            builder.setContentType(PrintDocumentInfo.CONTENT_TYPE_DOCUMENT)
                    .setPageCount(pageCount > 0 ? pageCount : PrintDocumentInfo.PAGE_COUNT_UNKNOWN)
                    .build();
            layoutResultCallback.onLayoutFinished(builder.build(),
                    !printAttributes1.equals(printAttributes));
        }
    }

    @Override
    public void onWrite(PageRange[] pageRanges, ParcelFileDescriptor parcelFileDescriptor, CancellationSignal cancellationSignal, WriteResultCallback writeResultCallback) {
        InputStream in = null;
        OutputStream out = null;
        try {
            if (resID != 0) {
                in = context.getResources().openRawResource(resID);
            } else if (pathName != null) {
                File file = new File(pathName);
                in = new FileInputStream(file);
            } else {
                writeResultCallback.onWriteFailed("Nothing to print");
                return;
            }
            out = new FileOutputStream(parcelFileDescriptor.getFileDescriptor());

            byte[] buf = new byte[16384];
            int size;

            while ((size = in.read(buf)) >= 0 && !cancellationSignal.isCanceled()) {
                out.write(buf, 0, size);
            }

            if (cancellationSignal.isCanceled()) {
                writeResultCallback.onWriteCancelled();
            } else {
                writeResultCallback.onWriteFinished(new PageRange[] { PageRange.ALL_PAGES });
            }
        }
        catch (Exception e) {
            writeResultCallback.onWriteFailed(e.getMessage());
            Log.e(LOGTAG, "onWrite: " + e);
        }
        finally {
            try {
                in.close();
                out.close();
            }
            catch (IOException e) {
                Log.e(LOGTAG, "close: " + e);
            }
        }
    }
}