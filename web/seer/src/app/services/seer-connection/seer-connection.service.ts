import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/Rx';
import { RxWebsocketSubject } from './connection/connection';

@Injectable()
export class SeerConnectionService {
  private connections: Map<string, RxWebsocketSubject<object>> = new Map<string, RxWebsocketSubject<object>>();
  // constructor() { }

  public connect(url: string): Observable<any> {
    if (this.connections.has(url)) {
      return this.connections.get(url);
    } else {
      const connection = new RxWebsocketSubject(url);
      this.connections.set(url, connection);
      return connection;
    }
  }

  public send(url: string, message: any): void {
    if (this.connections.has(url) === false) {
      throw new Error(`No connection by the url: ${url} exists, please call connect first`);
    }
    this.connections.get(url).send(message);
  }
}
